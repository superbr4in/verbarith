#include <catch2/catch.hpp>

#include <verbarith/expression.hpp>

using namespace vra;

// Type deduction
static_assert(std::is_same_v<decltype(expression(false                         )), expression<bool          >>);
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned char >(0))), expression<unsigned char >>);
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned short>(0))), expression<unsigned short>>);
static_assert(std::is_same_v<decltype(expression(static_cast<unsigned      >(0))), expression<unsigned      >>);

TEST_CASE("Expression: Symbol requirements")
{
    std::string const error_message("Invalid symbol");
    CHECK_THROWS_WITH(expression<unsigned char>(""),    error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("0"),   error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("0ab"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>(" "),   error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("a "),  error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("a b"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("c\t"), error_message);
    CHECK_THROWS_WITH(expression<unsigned char>("\n"),  error_message);
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
        expression<unsigned> const value("X");

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
        expression<unsigned> const value("X");

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
    CHECK(expression<unsigned char>(27).is_conclusive());
    CHECK_FALSE(expression<unsigned char>("X").is_conclusive());
}
TEST_CASE("Expression: Evaluation")
{
    CHECK(expression<unsigned char>(0).evaluate() == 0);
    CHECK(expression(static_cast<unsigned char>(599)).evaluate() == 87);
    CHECK_THROWS_WITH(expression<unsigned char>("X").evaluate(), "Inconclusive evaluation");
}

TEST_CASE("Expression: Equality")
{
    expression<unsigned> a("a");

    SECTION("Same")
    {
        CHECK      (a == a);
        CHECK_FALSE(a != a);
    }
    SECTION("Copy")
    {
        expression a_copy(a);

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
        expression<unsigned> const b("b");

        CHECK_FALSE(b == a);
        CHECK      (b != a);
    }
}

TEST_CASE("Expression: Conclusive EQ")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const c = a == b;

    CHECK(expression(a).equal(expression(b)).evaluate() == c);
}
TEST_CASE("Expression: Conclusive LT")
{
    auto const a = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
    auto const b = static_cast<unsigned char>(GENERATE(range(0x00, 0x08), range(0xF8, 0x100)));
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
