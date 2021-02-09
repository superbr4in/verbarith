#include <algorithm>

#include <verbarith/expression.hpp>
#include <verbarith/expression_operation.ipp>

namespace vra
{
    template <expression_typename T>
    template <typename... Arguments, expression_maker<Arguments...> Maker>
    expression<T>::expression(Maker&& maker, Arguments&&... arguments) :
        base_(std::invoke(std::forward<Maker>(maker), expression_context::instance(), std::forward<Arguments>(arguments)...))
    {
        Z3_inc_ref(expression_context::instance(), base_);

        simplify();
    }

    template <expression_typename T>
    template <typename... Arguments, expression_maker<_Z3_ast*, Arguments...> Maker>
    void expression<T>::apply(Maker&& maker, Arguments&&... arguments)
    {
        auto* const new_base = std::invoke(std::forward<Maker>(maker), expression_context::instance(), base_, std::forward<Arguments>(arguments)...);

        Z3_dec_ref(expression_context::instance(), base_);

        base_ = new_base;

        Z3_inc_ref(expression_context::instance(), base_);

        simplify();
    }
    template <expression_typename T>
    void expression<T>::simplify()
    {
        auto* const simplified_base = Z3_simplify(expression_context::instance(), base_);

        Z3_dec_ref(expression_context::instance(), base_);

        base_ = simplified_base;

        Z3_inc_ref(expression_context::instance(), base_);
    }

    template <expression_typename T>
    expression<T>::expression(T value) :
        base_(Z3_mk_unsigned_int64(expression_context::instance(), static_cast<std::uint64_t>(value), expression_sort<widthof(T)>::instance()))
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <expression_typename T>
    expression<T>::expression(std::string const& symbol) :
        base_(Z3_mk_const(expression_context::instance(), expression_symbol(symbol), expression_sort<widthof(T)>::instance()))
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");

        Z3_inc_ref(expression_context::instance(), base_);
    }

    template <expression_typename T>
    template <expression_typename U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U> const& other) :
        base_(other.base_)
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <expression_typename U>
        requires (widthof(U) > widthof(T))
    expression<T>::expression(expression<U> const& other) :
        expression(&Z3_mk_extract, unsigned{widthof(T) - 1}, unsigned{0}, other.base_)
    { }
    template <expression_typename T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <expression_typename U>
        requires (widthof(U) < widthof(T))
    expression<T>::expression(expression<U> const& other) :
        expression(&Z3_mk_zero_ext, unsigned{widthof(T) - widthof(U)}, other.base_)
    { }

    template <expression_typename T>
    expression<T>::~expression()
    {
        if (base_ != nullptr)
            Z3_dec_ref(expression_context::instance(), base_);
    }

    template <expression_typename T>
    expression<T>::expression(expression const& other) :
        base_(other.base_)
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator=(expression const& other)
    {
        if (&other != this)
        {
            if (base_ != nullptr)
                Z3_dec_ref(expression_context::instance(), base_);

            base_ = other.base_;

            Z3_inc_ref(expression_context::instance(), base_);
        }

        return *this;
    }

    template <expression_typename T>
    expression<T>::expression(expression&& other) noexcept :
        base_(std::exchange(other.base_, nullptr))
    { }
    template <expression_typename T>
    expression<T>& expression<T>::operator=(expression&& other) noexcept
    {
        if (&other != this)
        {
            if (base_ != nullptr)
                Z3_dec_ref(expression_context::instance(), base_);

            base_ = std::exchange(other.base_, nullptr);
        }

        return *this;
    }

    template <expression_typename T>
    unsigned expression<T>::arity() const
    {
        if (Z3_get_ast_kind(expression_context::instance(), base_) != Z3_APP_AST)
            throw std::logic_error("No operation");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return Z3_get_arity(expression_context::instance(), Z3_get_app_decl(expression_context::instance(), reinterpret_cast<_Z3_app*>(base_)));
    }

    template <expression_typename T>
    bool expression<T>::is_conclusive() const
    {
        return Z3_is_numeral_ast(expression_context::instance(), base_);
    }
    template <expression_typename T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; Z3_get_numeral_uint64(expression_context::instance(), base_, &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <expression_typename T>
    bool expression<T>::operator==(expression<T> const& other) const
    {
        return Z3_is_eq_ast(expression_context::instance(), base_, other.base_);
    }
    template <expression_typename T>
    bool expression<T>::operator!=(expression<T> const& other) const
    {
        return !operator==(other);
    }

    template <expression_typename T>
    template <expression_typename U, std::size_t POSITION>
        requires (widthof(T) >= widthof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const
    {
        return expression<U>(&Z3_mk_extract, unsigned{(widthof(U) * (POSITION + 1)) - 1}, unsigned{widthof(U) * POSITION}, base_);
    }

    template <expression_typename T>
    template <expression_typename U>
    expression<U> expression<T>::dereference() const
    {
        static expression_operation<widthof(U), widthof(T)> const indirection("deref" + std::to_string(widthof(U)));
        return expression<U>(&Z3_mk_app, indirection, 1, &base_);
    }

    template <expression_typename T>
    expression<bool> expression<T>::equal(expression const& other) const
    {
        expression<bool> eq(&Z3_mk_eq, base_, other.base_);

        expression<bool> one(true);
        expression<bool> zero(false);

        return expression<bool>(&Z3_mk_ite, eq.base_, one.base_, zero.base_);
    }
    template <expression_typename T>
    expression<bool> expression<T>::less_than(expression const& other) const
    {
        _Z3_ast* (* make)(_Z3_context*, _Z3_ast*, _Z3_ast*) { };
        if constexpr (std::signed_integral<T>)
        {
            make = &Z3_mk_bvslt;
        }
        else
        {
            make = &Z3_mk_bvult;
        }

        expression<bool> lt(make, base_, other.base_);

        expression<bool> one(true);
        expression<bool> zero(false);

        return expression<bool>(&Z3_mk_ite, lt.base_, one.base_, zero.base_);
    }

    template <expression_typename T>
    expression<T> expression<T>::operator-() const
    {
        return expression(&Z3_mk_bvneg, base_);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator~() const
    {
        return expression(&Z3_mk_bvnot, base_);
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator++()
    {
        expression<T> one(1);

        apply(&Z3_mk_bvadd, one.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator--()
    {
        expression<T> one(1);

        apply(&Z3_mk_bvsub, one.base_);

        return *this;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator+=(expression const& other)
    {
        apply(&Z3_mk_bvadd, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator+(expression<T> a, expression<T> const& b)
    {
        return a += b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator-=(expression const& other)
    {
        apply(&Z3_mk_bvsub, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator-(expression<T> a, expression<T> const& b)
    {
        return a -= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator*=(expression const& other)
    {
        apply(&Z3_mk_bvmul, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator*(expression<T> a, expression<T> const& b)
    {
        return a *= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator/=(expression const& other)
    {
        if constexpr (std::signed_integral<T>)
        {
            apply(&Z3_mk_bvsdiv, other.base_);
        }
        else
        {
            apply(&Z3_mk_bvudiv, other.base_);
        }

        return *this;
    }
    template <expression_typename T>
    expression<T> operator/(expression<T> a, expression<T> const& b)
    {
        return a /= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator%=(expression const& other)
    {
        if constexpr (std::signed_integral<T>)
        {
            apply(&Z3_mk_bvsrem, other.base_);
        }
        else
        {
            apply(&Z3_mk_bvurem, other.base_);
        }

        return *this;
    }
    template <expression_typename T>
    expression<T> operator%(expression<T> a, expression<T> const& b)
    {
        return a %= b;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator&=(expression const& other)
    {
        apply(&Z3_mk_bvand, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator&(expression<T> a, expression<T> const& b)
    {
        return a &= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator|=(expression const& other)
    {
        apply(&Z3_mk_bvor, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator|(expression<T> a, expression<T> const& b)
    {
        return a |= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator^=(expression const& other)
    {
        apply(&Z3_mk_bvxor, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator^(expression<T> a, expression<T> const& b)
    {
        return a ^= b;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator<<=(expression const& other)
    {
        apply(&Z3_mk_bvshl, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator<<(expression<T> a, expression<T> const& b)
    {
        return a <<= b;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator>>=(expression const& other)
    {
        apply(&Z3_mk_bvlshr, other.base_);

        return *this;
    }
    template <expression_typename T>
    expression<T> operator>>(expression<T> a, expression<T> const& b)
    {
        return a >>= b;
    }

    template <expression_typename T>
    std::ostream& operator<<(std::ostream& stream, expression<T> const& expression)
    {
        stream << Z3_ast_to_string(expression_context::instance(), expression.base_);

        return stream;
    }

    template <expression_typename T>
    template <expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts)
    {
        return join<0>(parts);
    }
    template <expression_typename T>
    template <std::size_t INDEX, expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts)
    {
        if constexpr (INDEX == widthof(T) / widthof(U) - 1)
        {
            auto const identity =
                [](_Z3_context* const, _Z3_ast* const base) // NOLINT [hicpp-named-parameter]
                {
                    return base;
                };
            return expression(identity, std::get<INDEX>(parts).base_);
        }
        else
        {
            return expression(&Z3_mk_concat, join<INDEX + 1>(parts).base_, std::get<INDEX>(parts).base_);
        }
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    template <vra::expression_typename T>
    std::size_t hash<vra::expression<T>>::operator()(vra::expression<T> const& expression) const noexcept
    {
        return Z3_get_ast_hash(vra::expression_context::instance(), expression.base_);
    }
}

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
