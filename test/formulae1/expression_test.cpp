#include <catch2/catch.hpp>

#include <formulae1/expression.hpp>

using namespace vra;

// Type deduction
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned char >(0))), expression<unsigned char >>);
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned short>(0))), expression<unsigned short>>);
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned int  >(0))), expression<unsigned int  >>);
static_assert(std::is_same_v<decltype(expression(static_cast<  signed int  >(0))), expression<  signed int  >>);

TEST_CASE("Expression: Symbol requirements")
{
    std::string const error_message("Invalid symbol");

    CHECK_THROWS_WITH(expression<unsigned char>::symbol(""),    error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("0"),   error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("0ab"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol(" "),   error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("a "),  error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("a b"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("c\t"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("\n"),  error_message);
}

TEST_CASE("Expression: Signedness")
{
    SECTION("Unsigned to signed")
    {
        expression<unsigned> value(0x12345678);
        expression<int> converted(0x12345678);

        SECTION("Copy")
        {
            CHECK(expression<int>(value) == converted);
        }
        SECTION("Move")
        {
            CHECK(expression<int>(std::move(value)) == converted);
        }
    }
    SECTION("Signed to unsigned")
    {
        expression<int> value(0x12345678);
        expression<unsigned> converted(0x12345678);

        SECTION("Copy")
        {
            CHECK(expression<unsigned>(value) == converted);
        }
        SECTION("Move")
        {
            CHECK(expression<unsigned>(std::move(value)) == converted);
        }
    }
}

TEST_CASE("Expression: Shrinkage")
{
    CHECK(expression<unsigned short>(expression<unsigned>(0x12345678)) == expression<unsigned short>(0x5678));
}
TEST_CASE("Expression: Growth")
{
    CHECK(expression<unsigned>(expression<unsigned short>(0x5678)) == expression<unsigned>(0x00005678));
}

TEST_CASE("Expression: Joint")
{
    SECTION("Itself")
    {
        auto const value = expression<unsigned>::symbol("X");

        CHECK(expression<unsigned>::join<unsigned>({ value }) == value);
    }
    SECTION("Arbitrary")
    {
        expression<unsigned> const result(0x12345678);

        /* Little endian */

        SECTION("Quadrupled")
        {
            CHECK(expression<unsigned>::join<unsigned char>(
                {
                    expression<unsigned char>(0x78),
                    expression<unsigned char>(0x56),
                    expression<unsigned char>(0x34),
                    expression<unsigned char>(0x12)
                }) == result);
        }
        SECTION("Doubled")
        {
            CHECK(expression<unsigned>::join<unsigned short>(
                {
                    expression<unsigned short>(0x5678),
                    expression<unsigned short>(0x1234)
                }) == result);
        }
    }
}
TEST_CASE("Expression: Extraction")
{
    SECTION("Itself")
    {
        auto const value = expression<unsigned>::symbol("X");

        CHECK(value.extract<unsigned, 0>() == value);
    }
    SECTION("Arbitrary")
    {
        expression<unsigned> const source(0x12345678);

        /* Little endian */

        SECTION("Quartered")
        {
            CHECK(source.extract<unsigned char, 0>() == expression<unsigned char>(0x78));
            CHECK(source.extract<unsigned char, 1>() == expression<unsigned char>(0x56));
            CHECK(source.extract<unsigned char, 2>() == expression<unsigned char>(0x34));
            CHECK(source.extract<unsigned char, 3>() == expression<unsigned char>(0x12));
        }
        SECTION("Halved")
        {
            CHECK(source.extract<unsigned short, 0>() == expression<unsigned short>(0x5678));
            CHECK(source.extract<unsigned short, 1>() == expression<unsigned short>(0x1234));
        }
    }
}

TEST_CASE("Expression: Conclusiveness")
{
    CHECK(expression<unsigned char>(27).conclusive());
    CHECK_FALSE(expression<unsigned char>::symbol("X").conclusive());
}

TEST_CASE("Expression: Dependencies")
{
    expression<unsigned char> const value_1(27);
    auto const value_1_dependencies = value_1.dependencies();
    REQUIRE(0 == value_1_dependencies.size());

    std::string const value_2_symbol("X");

    auto const value_2 = expression<unsigned char>::symbol(value_2_symbol);
    auto const value_2_dependencies = value_2.dependencies();
    REQUIRE(1 == value_2_dependencies.size());
    REQUIRE(value_2_symbol == *value_2_dependencies.begin());

    auto const value_3 = value_1 + value_2;
    auto const value_3_dependencies = value_3.dependencies();
    REQUIRE(1 == value_3_dependencies.size());
    REQUIRE(value_2_symbol == *value_3_dependencies.begin());

    auto const value_4 = value_3 * value_2;
    auto const value_4_dependencies = value_4.dependencies();
    REQUIRE(1 == value_4_dependencies.size());
    REQUIRE(value_2_symbol == *value_4_dependencies.begin());

    auto const value_5 = value_1 + value_2.dereference<unsigned char>();
    auto const value_5_dependencies = value_5.dependencies();
    REQUIRE(1 == value_5_dependencies.size());
    REQUIRE(value_2_symbol == *value_5_dependencies.begin());

    std::string const value_6_symbol("Y");

    auto const value_6 = expression<unsigned char>::symbol(value_6_symbol);

    auto const value_7 = value_4 - value_6;
    auto const value_7_dependencies = value_7.dependencies();
    REQUIRE(2 == value_7_dependencies.size());
    REQUIRE((
        (value_2_symbol == *value_7_dependencies.begin() && value_6_symbol == *std::next(value_7_dependencies.begin())) ||
        (value_6_symbol == *value_7_dependencies.begin() && value_2_symbol == *std::next(value_7_dependencies.begin()))));
}
TEST_CASE("Expression: Dependencies (indirect)")
{
    expression<unsigned char> const value_1(27);
    auto const value_1_dependencies = value_1.dependencies_indirect();
    REQUIRE(0 == value_1_dependencies.size());

    auto const value_2 = expression<unsigned char>::symbol("X");
    auto const value_2_dependencies = value_2.dependencies_indirect();
    REQUIRE(0 == value_2_dependencies.size());

    auto const value_3 = -value_2;
    auto const value_3_dependencies = value_3.dependencies_indirect();
    REQUIRE(0 == value_3_dependencies.size());

    auto const value_4 = value_3 * value_2;
    auto const value_4_dependencies = value_4.dependencies_indirect();
    REQUIRE(0 == value_4_dependencies.size());

    auto const value_5 = value_1 + value_2.dereference<unsigned char>();
    auto const value_5_dependencies = value_5.dependencies_indirect();
    REQUIRE(1 == value_5_dependencies.size());
    REQUIRE(expression<>(value_2) == *value_5_dependencies.begin());

    auto const value_6 = value_1.dereference<unsigned char>();
    auto const value_6_dependencies = value_6.dependencies_indirect();
    REQUIRE(1 == value_6_dependencies.size());
    REQUIRE(expression<>(value_1) == *value_6_dependencies.begin());
}

TEST_CASE("Expression: Evaluation")
{
    CHECK(expression<unsigned char>(0).evaluate() == 0);
    CHECK(expression<  signed char>(0).evaluate() == 0);
    CHECK(expression<unsigned char>(117).evaluate() == 117);
    CHECK(expression<  signed char>(117).evaluate() == 117);
    CHECK(expression<  signed char>(-44).evaluate() == -44);
    CHECK(expression(static_cast<unsigned char>(599)).evaluate() == 87);
    CHECK(expression(static_cast<  signed char>(234)).evaluate() == -22);
    CHECK_THROWS_WITH(expression<unsigned char>::symbol("X").evaluate(), "Inconclusive evaluation");
}

TEST_CASE("Expression: Equality")
{
    auto a = expression<unsigned>::symbol("a");

    SECTION("Same")
    {
        CHECK      (a == a);
        CHECK_FALSE(a != a);
    }
    SECTION("Copy")
    {
        expression<unsigned> a_copy(a);

        REQUIRE      (a_copy == a);
        REQUIRE_FALSE(a_copy != a);

        SECTION("Altered")
        {
            a += expression<unsigned>(1);

            REQUIRE_FALSE(a_copy == a);
            REQUIRE      (a_copy != a);

            a -= expression<unsigned>(1);

            CHECK      (a_copy == a);
            CHECK_FALSE(a_copy != a);
        }
        SECTION("Simplified")
        {
            a += expression<unsigned>(2);
            a_copy += expression<unsigned>(1);
            a_copy += expression<unsigned>(1);

            CHECK      (a_copy == a);
            CHECK_FALSE(a_copy != a);
        }
    }
    SECTION("Other")
    {
        auto const b = expression<unsigned>::symbol("b");

        CHECK_FALSE(b == a);
        CHECK      (b != a);
    }
}

TEST_CASE("Expression: Dereference")
{
    auto const value = expression<std::uint32_t>(4).dereference<std::uint8_t>();
    CHECK_FALSE(value.conclusive());
}

TEST_CASE("Expression: Conclusive EQ")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = a == b;

    CHECK(expression(a).equals(expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive LT")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = a < b;

    CHECK(expression(a).less_than(expression(b)).evaluate() == c);
}

TEST_CASE("Expression: Conclusive SLT")
{
    auto const a = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = a < b;

    CHECK(expression(a).less_than(expression(b)).evaluate() == c);
}

TEST_CASE("Expression: Conclusive NEG")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(-a);

    CHECK((-expression(a)).evaluate() == b);
}
TEST_CASE("Expression: Conclusive NOT")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(~a);

    CHECK((~expression(a)).evaluate() == b);
}

TEST_CASE("Expression: Conclusive INC")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(a + 1);

    CHECK((++expression(a)).evaluate() == b);
}
TEST_CASE("Expression: Conclusive DEC")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(a - 1);

    CHECK((--expression(a)).evaluate() == b);
}

TEST_CASE("Expression: Conclusive ADD")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a + b);

    CHECK((expression(a) + expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive SUB")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a - b);

    CHECK((expression(a) - expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive MUL")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a * b);

    CHECK((expression(a) * expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive DIV")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x01, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a / b);

    CHECK((expression(a) / expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive MOD")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x01, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a % b);

    CHECK((expression(a) % expression(b)).evaluate() == c);
}

TEST_CASE("Expression: Conclusive SMUL")
{
    auto const a = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<signed char>(a * b);

    CHECK((expression(a) * expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive SDIV")
{
    auto const a = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<signed char>(GENERATE(range(0x01, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<signed char>(a / b);

    CHECK((expression(a) / expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive SMOD")
{
    auto const a = static_cast<signed char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<signed char>(GENERATE(range(0x01, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<signed char>(a % b);

    CHECK((expression(a) % expression(b)).evaluate() == c);
}

TEST_CASE("Expression: Conclusive AND")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a & b);

    CHECK((expression(a) & expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive OR")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a | b);

    CHECK((expression(a) | expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive XOR")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a ^ b);

    CHECK((expression(a) ^ expression(b)).evaluate() == c);
}

TEST_CASE("Expression: Conclusive SHL")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a << b);

    CHECK((expression(a) << expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive SHR")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = static_cast<unsigned char>(a >> b);

    CHECK((expression(a) >> expression(b)).evaluate() == c);
}
