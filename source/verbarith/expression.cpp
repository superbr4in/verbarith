#include <algorithm>

#include <verbarith/expression.hpp>
#include <verbarith/expression_base.ipp>
#include <verbarith/expression_operation.ipp>
#include <verbarith/preprocessor_types.hpp>

namespace vra
{
    template <expression_typename T>
    static expression<T> const zero(0);
    template <expression_typename T>
    static expression<T> const one(1);

    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    expression<T>::expression(T const value) noexcept :
        expression(Z3_mk_unsigned_int64, static_cast<std::uint64_t>(value), expression_sort::instance<widthof(T)>())
    { }
    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    expression<T>::expression(std::string const& symbol) :
        expression(Z3_mk_const, Z3_mk_string_symbol(context(), symbol.c_str()), expression_sort::instance<widthof(T)>())
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");
    }

    template <expression_typename T>
    template <expression_typename U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression<>(other)
    { }
    template <expression_typename T>
    expression<T>::expression(expression<> const& other) noexcept :
        expression<>(other)
    {
        // TODO: Validation
    }
    template <expression_typename T>
    template <expression_typename U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U>&& other) noexcept :
        expression<>(std::move(other))
    { }
    template <expression_typename T>
    expression<T>::expression(expression<>&& other) noexcept :
        expression<>(std::move(other))
    {
        // TODO: Validation
    }

    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <expression_typename U>
        requires (widthof(U) > widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(Z3_mk_extract, unsigned{widthof(T) - 1}, unsigned{0}, other.resource())
    {
        apply(Z3_simplify);
    }
    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <expression_typename U>
        requires (widthof(U) < widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(Z3_mk_zero_ext, unsigned{widthof(T) - widthof(U)}, other.resource())
    {
        apply(Z3_simplify);
    }

    template <expression_typename T>
    unsigned expression<T>::arity() const
    {
        if (Z3_get_ast_kind(context(), resource()) != Z3_APP_AST)
            throw std::logic_error("No operation");

        return Z3_get_arity(context(), expression_operation(resource()));
    }

    template <expression_typename T>
    bool expression<T>::is_conclusive() const noexcept
    {
        return Z3_is_numeral_ast(context(), resource());
    }
    template <expression_typename T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; Z3_get_numeral_uint64(context(), resource(), &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <expression_typename T>
    template <expression_typename U, std::size_t POSITION>
        requires (widthof(T) >= widthof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const noexcept
    {
        expression<U> result(Z3_mk_extract, unsigned{(widthof(U) * (POSITION + 1)) - 1}, unsigned{widthof(U) * POSITION}, resource());
        result.apply(Z3_simplify);

        return result;
    }

    template <expression_typename T>
    template <expression_typename U>
        requires (!std::same_as<U, void>)
    expression<U> expression<T>::dereference() const noexcept
    {
        static auto const indirection = expression_operation::create<widthof(U), widthof(T)>("deref" + std::to_string(widthof(U)));

        auto* const resource = expression_base::resource();
        return expression<U>(Z3_mk_app, indirection, 1, &resource);
    }

    template <expression_typename T>
    expression<bool> expression<T>::equal(expression const& other) const noexcept
    {
        expression<bool> result(Z3_mk_ite, expression(Z3_mk_eq, resource(), other.resource()).resource(), one<bool>.resource(), zero<bool>.resource());
        result.apply(Z3_simplify);

        return result;
    }
    template <expression_typename T>
    expression<bool> expression<T>::less(expression const& other) const noexcept
    {
        auto* const maker = std::signed_integral<T> ? Z3_mk_bvslt : Z3_mk_bvult;

        expression<bool> result(Z3_mk_ite, expression(maker, resource(), other.resource()).resource(), one<bool>.resource(), zero<bool>.resource());
        result.apply(Z3_simplify);

        return result;
    }

    template <expression_typename T>
    expression<T> expression<T>::operator-() const noexcept
    {
        expression<T> result(Z3_mk_bvneg, resource());
        result.apply(Z3_simplify);

        return result;
    }
    template <expression_typename T>
    expression<T> expression<T>::operator~() const noexcept
    {
        expression<T> result(Z3_mk_bvnot, resource());
        result.apply(Z3_simplify);

        return result;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator++() noexcept
    {
        apply(Z3_mk_bvadd, one<T>.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator--() noexcept
    {
        apply(Z3_mk_bvsub, one<T>.resource());
        apply(Z3_simplify);

        return *this;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator+=(expression const& other) noexcept
    {
        apply(Z3_mk_bvadd, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator+(expression<T> a, expression<T> const& b) noexcept
    {
        return a += b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator-=(expression const& other) noexcept
    {
        apply(Z3_mk_bvsub, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator-(expression<T> a, expression<T> const& b) noexcept
    {
        return a -= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator*=(expression const& other) noexcept
    {
        apply(Z3_mk_bvmul, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator*(expression<T> a, expression<T> const& b) noexcept
    {
        return a *= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator/=(expression const& other) noexcept
    {
        auto* const maker = std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv;

        apply(maker, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator/(expression<T> a, expression<T> const& b) noexcept
    {
        return a /= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator%=(expression const& other) noexcept
    {
        auto* const maker = std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem;

        apply(maker, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator%(expression<T> a, expression<T> const& b) noexcept
    {
        return a %= b;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator&=(expression const& other) noexcept
    {
        apply(Z3_mk_bvand, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator&(expression<T> a, expression<T> const& b) noexcept
    {
        return a &= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator|=(expression const& other) noexcept
    {
        apply(Z3_mk_bvor, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator|(expression<T> a, expression<T> const& b) noexcept
    {
        return a |= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator^=(expression const& other) noexcept
    {
        apply(Z3_mk_bvxor, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator^(expression<T> a, expression<T> const& b) noexcept
    {
        return a ^= b;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator<<=(expression const& other) noexcept
    {
        apply(Z3_mk_bvshl, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator<<(expression<T> a, expression<T> const& b) noexcept
    {
        return a <<= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator>>=(expression const& other) noexcept
    {
        apply(Z3_mk_bvlshr, other.resource());
        apply(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator>>(expression<T> a, expression<T> const& b) noexcept
    {
        return a >>= b;
    }

    template <expression_typename T>
    template <expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts) noexcept
    {
        if constexpr (widthof(T) == widthof(U))
        {
            return expression<T>(std::get<0>(parts));
        }
        else
        {
            auto result = join<0>(parts);
            result.apply(Z3_simplify);

            return result;
        }
    }
    template <expression_typename T>
    template <std::size_t INDEX, expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts) noexcept
    {
        if constexpr (INDEX == widthof(T) / widthof(U) - 2)
        {
            return expression<T>(Z3_mk_concat, std::get<INDEX + 1>(parts).resource(), std::get<INDEX>(parts).resource());
        }
        else
        {
            return expression<T>(Z3_mk_concat, join<INDEX + 1>(parts).resource(), std::get<INDEX>(parts).resource());
        }
    }

    template <expression_typename T>
    bool operator==(expression<T> const& value_1, expression<T> const& value_2) noexcept
    {
        return Z3_is_eq_ast(expression<T>::context(), value_1.resource(), value_2.resource());
    }
    template <expression_typename T>
    std::ostream& operator<<(std::ostream& stream, expression<T> const& value) noexcept
    {
        stream << Z3_ast_to_string(expression<T>::context(), value.resource());

        return stream;
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    template <vra::expression_typename T>
    std::size_t hash<vra::expression<T>>::operator()(vra::expression<T> const& value) const noexcept
    {
        static hash<vra::expression<>> const base_hasher;
        return base_hasher(value);
    }
    template <>
    std::size_t hash<vra::expression<>>::operator()(vra::expression<> const& value) const noexcept
    {
        return Z3_get_ast_hash(vra::expression<>::context(), value.resource());
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

template bool vra::operator==(expression<> const&, expression<> const&);
template std::ostream& vra::operator<<(std::ostream&, expression<> const&);

#define INSTANTIATE_2(T, U, index)\
    template                                              vra::EXPRESSION(U) vra::EXPRESSION(T)::extract<TYPE(U), index>() const
#define INSTANTIATE_1(T, U)\
    IF_NOT_EQUAL(T, U, IF_TYPE_WIDTH_EQUAL(T, U, template                    vra::EXPRESSION(T)::expression(     EXPRESSION(U)&&)));\
    IF_NOT_EQUAL(T, U, template                                              vra::EXPRESSION(T)::expression(     EXPRESSION(U) const&));\
    template                                              vra::EXPRESSION(U) vra::EXPRESSION(T)::dereference() const;\
    IF_TYPE_WIDTH_DIVIDABLE(T, U, template                vra::EXPRESSION(T) vra::EXPRESSION(T)::join(std::array<EXPRESSION(U), TYPE_WIDTH_DIVIDE(T, U)> const&));\
    LOOP_TYPE_WIDTH_DIVIDE_2(T, U, INSTANTIATE_2, T, U)
#define INSTANTIATE_0(T)\
    template                                              class              vra::EXPRESSION(T);\
    template                                              bool               vra::operator==(EXPRESSION(T) const&, EXPRESSION(T) const&);\
    template                                              std::ostream&      vra::operator<<(std::ostream&       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator+ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator- (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator* (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator/ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator% (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator& (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator| (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator^ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator<<(EXPRESSION(T)       , EXPRESSION(T) const&);\
    template                                              vra::EXPRESSION(T) vra::operator>>(EXPRESSION(T)       , EXPRESSION(T) const&);\
    LOOP_TYPES_1(INSTANTIATE_1, T)
LOOP_TYPES_0(INSTANTIATE_0);

#define INSTANTIATE_HASH(T)\
    template class std::hash<vra::EXPRESSION(T)>
LOOP_TYPES_0(INSTANTIATE_HASH);
