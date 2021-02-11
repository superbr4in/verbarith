#include <algorithm>

#include <verbarith/expression.hpp>
#include <verbarith/expression_base.ipp>
#include <verbarith/expression_operation.ipp>

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

template bool vra::operator==(expression<> const&, expression<> const&);
template std::ostream& vra::operator<<(std::ostream&, expression<> const&);

// NOLINTNEXTLINE [cppcoreguidelines-macro-usage]
#define INSTANTIATE(T, U1, U2, U3, U4, U5, U6, U7, U8)\
    template class               vra::expression<T>;\
    template                     vra::expression<T>::expression(expression<U1> const&);\
    template                     vra::expression<T>::expression(expression<U2> const&);\
    template                     vra::expression<T>::expression(expression<U3> const&);\
    template                     vra::expression<T>::expression(expression<U4> const&);\
    template                     vra::expression<T>::expression(expression<U5> const&);\
    template                     vra::expression<T>::expression(expression<U6> const&);\
    template                     vra::expression<T>::expression(expression<U7> const&);\
    template                     vra::expression<T>::expression(expression<U8> const&);\
    template vra::expression<T > vra::expression<T>::dereference() const;\
    template vra::expression<U1> vra::expression<T>::dereference() const;\
    template vra::expression<U2> vra::expression<T>::dereference() const;\
    template vra::expression<U3> vra::expression<T>::dereference() const;\
    template vra::expression<U4> vra::expression<T>::dereference() const;\
    template vra::expression<U5> vra::expression<T>::dereference() const;\
    template vra::expression<U6> vra::expression<T>::dereference() const;\
    template vra::expression<U7> vra::expression<T>::dereference() const;\
    template vra::expression<U8> vra::expression<T>::dereference() const;\
    template bool                               vra::operator==(expression<T > const&, expression<T > const&);\
    template std::ostream&                      vra::operator<<(std::ostream&        , expression<T > const&);\
    template vra::expression<T >                vra::operator+ (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator- (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator* (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator/ (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator% (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator& (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator| (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator^ (expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator<<(expression<T >       , expression<T > const&);\
    template vra::expression<T >                vra::operator>>(expression<T >       , expression<T > const&)

INSTANTIATE(bool         ,       std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std:: int8_t , bool,              std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std::uint8_t , bool, std::int8_t,               std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std:: int16_t, bool, std::int8_t, std::uint8_t,               std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std::uint16_t, bool, std::int8_t, std::uint8_t, std::int16_t,                std::int32_t, std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std:: int32_t, bool, std::int8_t, std::uint8_t, std::int16_t, std::uint16_t,               std::uint32_t, std::int64_t, std::uint64_t);
INSTANTIATE(std::uint32_t, bool, std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,                std::int64_t, std::uint64_t);
INSTANTIATE(std:: int64_t, bool, std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t,               std::uint64_t);
INSTANTIATE(std::uint64_t, bool, std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t               );

template vra::expression<bool         > vra::expression<bool         >::join(std::array<expression<bool         >,  1> const&);
template vra::expression<bool         > vra::expression<bool         >::extract                   <bool          ,  0>() const;

template                                vra::expression<std:: int8_t >::expression(expression     <std::uint8_t >&&);
template vra::expression<std:: int8_t > vra::expression<std:: int8_t >::join(std::array<expression<bool         >,  8> const&);
template vra::expression<std:: int8_t > vra::expression<std:: int8_t >::join(std::array<expression<std:: int8_t >,  1> const&);
template vra::expression<std:: int8_t > vra::expression<std:: int8_t >::join(std::array<expression<std::uint8_t >,  1> const&);
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std:: int8_t >::extract                   <bool          ,  7>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int8_t >::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int8_t >::extract                   <std::uint8_t  ,  0>() const;

template                                vra::expression<std::uint8_t >::expression(expression     <std:: int8_t >&&);
template vra::expression<std::uint8_t > vra::expression<std::uint8_t >::join(std::array<expression<bool         >,  8> const&);
template vra::expression<std::uint8_t > vra::expression<std::uint8_t >::join(std::array<expression<std:: int8_t >,  1> const&);
template vra::expression<std::uint8_t > vra::expression<std::uint8_t >::join(std::array<expression<std::uint8_t >,  1> const&);
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std::uint8_t >::extract                   <bool          ,  7>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint8_t >::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint8_t >::extract                   <std::uint8_t  ,  0>() const;

template                                vra::expression<std:: int16_t>::expression(expression     <std::uint16_t>&&);
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::join(std::array<expression<bool         >, 16> const&);
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::join(std::array<expression<std:: int8_t >,  2> const&);
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::join(std::array<expression<std::uint8_t >,  2> const&);
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::join(std::array<expression<std:: int16_t>,  1> const&);
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::join(std::array<expression<std::uint16_t>,  1> const&);
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std:: int16_t>::extract                   <bool          , 15>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int16_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int16_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int16_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int16_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int16_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int16_t>::extract                   <std::uint16_t ,  0>() const;

template                                vra::expression<std::uint16_t>::expression(expression     <std:: int16_t>&&);
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::join(std::array<expression<bool         >, 16> const&);
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::join(std::array<expression<std:: int8_t >,  2> const&);
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::join(std::array<expression<std::uint8_t >,  2> const&);
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::join(std::array<expression<std:: int16_t>,  1> const&);
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::join(std::array<expression<std::uint16_t>,  1> const&);
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std::uint16_t>::extract                   <bool          , 15>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint16_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint16_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint16_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint16_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint16_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint16_t>::extract                   <std::uint16_t ,  0>() const;

template                                vra::expression<std:: int32_t>::expression(expression     <std::uint32_t>&&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<bool         >, 32> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std:: int8_t >,  4> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std::uint8_t >,  4> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std:: int16_t>,  2> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std::uint16_t>,  2> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std:: int32_t>,  1> const&);
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::join(std::array<expression<std::uint32_t>,  1> const&);
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         > vra::expression<std:: int32_t>::extract                   <bool          , 31>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int32_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int32_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int32_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int32_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int32_t> vra::expression<std:: int32_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t> vra::expression<std:: int32_t>::extract                   <std::uint32_t ,  0>() const;

template                                vra::expression<std::uint32_t>::expression(expression     <std:: int32_t>&&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<bool         >, 32> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std:: int8_t >,  4> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std::uint8_t >,  4> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std:: int16_t>,  2> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std::uint16_t>,  2> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std:: int32_t>,  1> const&);
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::join(std::array<expression<std::uint32_t>,  1> const&);
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         > vra::expression<std::uint32_t>::extract                   <bool          , 31>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint32_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint32_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint32_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint32_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int32_t> vra::expression<std::uint32_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t> vra::expression<std::uint32_t>::extract                   <std::uint32_t ,  0>() const;

template                                vra::expression<std:: int64_t>::expression(expression     <std::uint64_t>&&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<bool         >, 64> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std:: int8_t >,  8> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std::uint8_t >,  8> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std:: int16_t>,  4> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std::uint16_t>,  4> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std:: int32_t>,  2> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std::uint32_t>,  2> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std:: int64_t>,  1> const&);
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::join(std::array<expression<std::uint64_t>,  1> const&);
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 31>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 32>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 33>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 34>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 35>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 36>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 37>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 38>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 39>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 40>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 41>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 42>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 43>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 44>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 45>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 46>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 47>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 48>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 49>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 50>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 51>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 52>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 53>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 54>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 55>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 56>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 57>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 58>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 59>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 60>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 61>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 62>() const;
template vra::expression<bool         > vra::expression<std:: int64_t>::extract                   <bool          , 63>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  4>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  4>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  5>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  5>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  6>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  6>() const;
template vra::expression<std:: int8_t > vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  7>() const;
template vra::expression<std::uint8_t > vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  7>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  2>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  2>() const;
template vra::expression<std:: int16_t> vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  3>() const;
template vra::expression<std::uint16_t> vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  3>() const;
template vra::expression<std:: int32_t> vra::expression<std:: int64_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t> vra::expression<std:: int64_t>::extract                   <std::uint32_t ,  0>() const;
template vra::expression<std:: int32_t> vra::expression<std:: int64_t>::extract                   <std:: int32_t ,  1>() const;
template vra::expression<std::uint32_t> vra::expression<std:: int64_t>::extract                   <std::uint32_t ,  1>() const;
template vra::expression<std:: int64_t> vra::expression<std:: int64_t>::extract                   <std:: int64_t ,  0>() const;
template vra::expression<std::uint64_t> vra::expression<std:: int64_t>::extract                   <std::uint64_t ,  0>() const;

template                                vra::expression<std::uint64_t>::expression(expression     <std:: int64_t>&&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<bool         >, 64> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std:: int8_t >,  8> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std::uint8_t >,  8> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std:: int16_t>,  4> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std::uint16_t>,  4> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std:: int32_t>,  2> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std::uint32_t>,  2> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std:: int64_t>,  1> const&);
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::join(std::array<expression<std::uint64_t>,  1> const&);
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 31>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 32>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 33>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 34>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 35>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 36>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 37>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 38>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 39>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 40>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 41>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 42>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 43>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 44>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 45>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 46>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 47>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 48>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 49>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 50>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 51>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 52>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 53>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 54>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 55>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 56>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 57>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 58>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 59>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 60>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 61>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 62>() const;
template vra::expression<bool         > vra::expression<std::uint64_t>::extract                   <bool          , 63>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  4>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  4>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  5>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  5>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  6>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  6>() const;
template vra::expression<std:: int8_t > vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  7>() const;
template vra::expression<std::uint8_t > vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  7>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  2>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  2>() const;
template vra::expression<std:: int16_t> vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  3>() const;
template vra::expression<std::uint16_t> vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  3>() const;
template vra::expression<std:: int32_t> vra::expression<std::uint64_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t> vra::expression<std::uint64_t>::extract                   <std::uint32_t ,  0>() const;
template vra::expression<std:: int32_t> vra::expression<std::uint64_t>::extract                   <std:: int32_t ,  1>() const;
template vra::expression<std::uint32_t> vra::expression<std::uint64_t>::extract                   <std::uint32_t ,  1>() const;
template vra::expression<std:: int64_t> vra::expression<std::uint64_t>::extract                   <std:: int64_t ,  0>() const;
template vra::expression<std::uint64_t> vra::expression<std::uint64_t>::extract                   <std::uint64_t ,  0>() const;

template class std::hash<vra::expression<bool         >>;
template class std::hash<vra::expression<std:: int8_t >>;
template class std::hash<vra::expression<std::uint8_t >>;
template class std::hash<vra::expression<std:: int16_t>>;
template class std::hash<vra::expression<std::uint16_t>>;
template class std::hash<vra::expression<std:: int32_t>>;
template class std::hash<vra::expression<std::uint32_t>>;
template class std::hash<vra::expression<std:: int64_t>>;
template class std::hash<vra::expression<std::uint64_t>>;
