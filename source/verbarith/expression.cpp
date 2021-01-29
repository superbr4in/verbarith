#include <algorithm>
#include <ostream>

#include <stimpak/fixed_algorithm.hpp>

#include <verbarith/expression.hpp>
#include <verbarith/expression_context.hpp>
#include <verbarith/expression_sort.hpp>

namespace vra
{
    template <sti::decayed_integral T>
    template <sti::decayed... Arguments>
    expression<T>::expression(_Z3_ast* (* const make)(_Z3_context*, Arguments...), Arguments... arguments) :
        base_(make(expression_context::instance(), arguments...))
    {
        Z3_inc_ref(expression_context::instance(), base_);

        simplify();
    }

    template <sti::decayed_integral T>
    template <sti::decayed... Arguments>
    void expression<T>::apply(_Z3_ast* (* const make)(_Z3_context*, _Z3_ast*, Arguments...), Arguments... arguments)
    {
        auto* const new_base = make(expression_context::instance(), base_, arguments...);

        Z3_dec_ref(expression_context::instance(), base_);

        base_ = new_base;

        Z3_inc_ref(expression_context::instance(), base_);

        simplify();
    }
    template <sti::decayed_integral T>
    void expression<T>::simplify()
    {
        auto* const simplified_base = Z3_simplify(expression_context::instance(), base_);

        Z3_dec_ref(expression_context::instance(), base_);

        base_ = simplified_base;

        Z3_inc_ref(expression_context::instance(), base_);
    }

    template <sti::decayed_integral T>
    expression<T>::expression(T value) :
        base_(Z3_mk_unsigned_int64(expression_context::instance(), static_cast<std::uint64_t>(value), expression_sort<T>()))
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <sti::decayed_integral T>
    expression<T>::expression(expression_symbol const& symbol) :
        base_(Z3_mk_const(expression_context::instance(), Z3_mk_string_symbol(expression_context::instance(), symbol.c_str()), expression_sort<T>()))
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");

        Z3_inc_ref(expression_context::instance(), base_);
    }

    template <sti::decayed_integral T>
    template <sti::decayed_integral U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U> const& other) :
        base_(other.base_)
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <sti::decayed_integral T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <sti::decayed_integral U>
        requires (widthof(U) > widthof(T))
    expression<T>::expression(expression<U> const& other) :
        expression(&Z3_mk_extract, unsigned{widthof(T) - 1}, unsigned{0}, other.base_)
    { }
    template <sti::decayed_integral T> // NOLINT [cppcoreguidelines-pro-type-member-init]
    template <sti::decayed_integral U>
        requires (widthof(U) < widthof(T))
    expression<T>::expression(expression<U> const& other) :
        expression(&Z3_mk_zero_ext, unsigned{widthof(T) - widthof(U)}, other.base_)
    { }

    template <sti::decayed_integral T>
    expression<T>::~expression()
    {
        if (base_ != nullptr)
            Z3_dec_ref(expression_context::instance(), base_);
    }

    template <sti::decayed_integral T>
    expression<T>::expression(expression const& other) :
        base_(other.base_)
    {
        Z3_inc_ref(expression_context::instance(), base_);
    }
    template <sti::decayed_integral T>
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

    template <sti::decayed_integral T>
    expression<T>::expression(expression&& other) noexcept :
        base_(std::exchange(other.base_, nullptr))
    { }
    template <sti::decayed_integral T>
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

    template <sti::decayed_integral T>
    template <sti::decayed_integral U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts)
    {
        if constexpr (widthof(T) == widthof(U))
        {
            return expression(std::get<0>(parts));
        }
        else
        {
            expression<T> joined(&Z3_mk_concat, std::get<1>(parts).base_, std::get<0>(parts).base_);
            sti::array_for_each(parts,
                [&joined]<std::size_t INDEX>(expression<U> const& part)
                {
                    if constexpr (INDEX > 1)
                    {
                        joined = expression<T>(&Z3_mk_concat, part.base_, joined.base_);
                    }
                });

            return joined;
        }
    }

    template <sti::decayed_integral T>
    template <sti::decayed_integral U, std::size_t POSITION>
        requires (widthof(T) >= widthof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const
    {
        return expression<U>(&Z3_mk_extract, unsigned{(widthof(U) * (POSITION + 1)) - 1}, unsigned{widthof(U) * POSITION}, base_);
    }

    template <sti::decayed_integral T>
    bool expression<T>::is_conclusive() const
    {
        return Z3_is_numeral_ast(expression_context::instance(), base_);
    }
    template <sti::decayed_integral T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; Z3_get_numeral_uint64(expression_context::instance(), base_, &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <sti::decayed_integral T>
    bool expression<T>::operator==(expression<T> const& other) const
    {
        return Z3_is_eq_ast(expression_context::instance(), base_, other.base_);
    }
    template <sti::decayed_integral T>
    bool expression<T>::operator!=(expression<T> const& other) const
    {
        return !operator==(other);
    }

    template <sti::decayed_integral T>
    expression<bool> expression<T>::equal(expression const& other) const
    {
        expression<bool> eq(&Z3_mk_eq, base_, other.base_);

        expression<bool> one(true);
        expression<bool> zero(false);

        return expression<bool>(&Z3_mk_ite, eq.base_, one.base_, zero.base_);
    }
    template <sti::decayed_integral T>
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

    template <sti::decayed_integral T>
    expression<T> expression<T>::operator-() const
    {
        return expression(&Z3_mk_bvneg, base_);
    }
    template <sti::decayed_integral T>
    expression<T> expression<T>::operator~() const
    {
        return expression(&Z3_mk_bvnot, base_);
    }

    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator++()
    {
        expression<T> one(1);

        apply(&Z3_mk_bvadd, one.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator--()
    {
        expression<T> one(1);

        apply(&Z3_mk_bvsub, one.base_);

        return *this;
    }

    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator+=(expression const& other)
    {
        apply(&Z3_mk_bvadd, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator+(expression<T> a, expression<T> const& b)
    {
        return a += b;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator-=(expression const& other)
    {
        apply(&Z3_mk_bvsub, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator-(expression<T> a, expression<T> const& b)
    {
        return a -= b;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator*=(expression const& other)
    {
        apply(&Z3_mk_bvmul, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator*(expression<T> a, expression<T> const& b)
    {
        return a *= b;
    }
    template <sti::decayed_integral T>
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
    template <sti::decayed_integral T>
    expression<T> operator/(expression<T> a, expression<T> const& b)
    {
        return a /= b;
    }
    template <sti::decayed_integral T>
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
    template <sti::decayed_integral T>
    expression<T> operator%(expression<T> a, expression<T> const& b)
    {
        return a %= b;
    }

    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator&=(expression const& other)
    {
        apply(&Z3_mk_bvand, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator&(expression<T> a, expression<T> const& b)
    {
        return a &= b;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator|=(expression const& other)
    {
        apply(&Z3_mk_bvor, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator|(expression<T> a, expression<T> const& b)
    {
        return a |= b;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator^=(expression const& other)
    {
        apply(&Z3_mk_bvxor, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator^(expression<T> a, expression<T> const& b)
    {
        return a ^= b;
    }

    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator<<=(expression const& other)
    {
        apply(&Z3_mk_bvshl, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator<<(expression<T> a, expression<T> const& b)
    {
        return a <<= b;
    }
    template <sti::decayed_integral T>
    expression<T>& expression<T>::operator>>=(expression const& other)
    {
        apply(&Z3_mk_bvlshr, other.base_);

        return *this;
    }
    template <sti::decayed_integral T>
    expression<T> operator>>(expression<T> a, expression<T> const& b)
    {
        return a >>= b;
    }

    template <sti::decayed_integral T>
    std::ostream& operator<<(std::ostream& stream, expression<T> const& expression)
    {
        stream << Z3_ast_to_string(expression_context::instance(), expression.base_);

        return stream;
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    template <sti::decayed_integral T>
    std::size_t hash<vra::expression<T>>::operator()(vra::expression<T> const& expression) const noexcept
    {
        return Z3_get_ast_hash(vra::expression_context::instance(), expression.base_);
    }
}

template class                                     vra::expression<bool         >;
template                                           vra::expression<bool         >::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<bool         >::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<bool         >::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<bool         >::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<bool         >::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<bool         >::expression(     expression<std::uint32_t     > const&);
template vra::expression<bool         >            vra::expression<bool         >::join(std::array<expression<bool         >,  1> const&);
template vra::expression<bool         >            vra::expression<bool         >::extract                   <bool          ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator+ (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator- (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator* (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator/ (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator% (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator& (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator| (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator^ (expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator<<(expression<bool         >,                 expression<bool              > const&);
template vra::expression<bool         > vra::operator>>(expression<bool         >,                 expression<bool              > const&);

template class                                     vra::expression<std:: int8_t >;
template                                           vra::expression<std:: int8_t >::expression(     expression<bool              > const&);
template                                           vra::expression<std:: int8_t >::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std:: int8_t >::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std:: int8_t >::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std:: int8_t >::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std:: int8_t >::expression(     expression<std::uint32_t     > const&);
template vra::expression<std:: int8_t >            vra::expression<std:: int8_t >::join(std::array<expression<bool         >,  8> const&);
template vra::expression<std:: int8_t >            vra::expression<std:: int8_t >::join(std::array<expression<std:: int8_t >,  1> const&);
template vra::expression<std:: int8_t >            vra::expression<std:: int8_t >::join(std::array<expression<std::uint8_t >,  1> const&);
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std:: int8_t >::extract                   <bool          ,  7>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int8_t >::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int8_t >::extract                   <std::uint8_t  ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator+ (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator- (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator* (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator/ (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator% (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator& (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator| (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator^ (expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator<<(expression<std:: int8_t >,                 expression<std:: int8_t      > const&);
template vra::expression<std:: int8_t > vra::operator>>(expression<std:: int8_t >,                 expression<std:: int8_t      > const&);

template class                                     vra::expression<std::uint8_t >;
template                                           vra::expression<std::uint8_t >::expression(     expression<bool              > const&);
template                                           vra::expression<std::uint8_t >::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std::uint8_t >::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std::uint8_t >::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std::uint8_t >::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std::uint8_t >::expression(     expression<std::uint32_t     > const&);
template vra::expression<std::uint8_t >            vra::expression<std::uint8_t >::join(std::array<expression<bool         >,  8> const&);
template vra::expression<std::uint8_t >            vra::expression<std::uint8_t >::join(std::array<expression<std:: int8_t >,  1> const&);
template vra::expression<std::uint8_t >            vra::expression<std::uint8_t >::join(std::array<expression<std::uint8_t >,  1> const&);
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std::uint8_t >::extract                   <bool          ,  7>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint8_t >::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint8_t >::extract                   <std::uint8_t  ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator+ (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator- (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator* (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator/ (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator% (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator& (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator| (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator^ (expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator<<(expression<std::uint8_t >,                 expression<std::uint8_t      > const&);
template vra::expression<std::uint8_t > vra::operator>>(expression<std::uint8_t >,                 expression<std::uint8_t      > const&);

template class                                     vra::expression<std:: int16_t>;
template                                           vra::expression<std:: int16_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std:: int16_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std:: int16_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std:: int16_t>::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std:: int16_t>::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std:: int16_t>::expression(     expression<std::uint32_t     > const&);
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::join(std::array<expression<bool         >, 16> const&);
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::join(std::array<expression<std:: int8_t >,  2> const&);
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::join(std::array<expression<std::uint8_t >,  2> const&);
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::join(std::array<expression<std:: int16_t>,  1> const&);
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::join(std::array<expression<std::uint16_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std:: int16_t>::extract                   <bool          , 15>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int16_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int16_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int16_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int16_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int16_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int16_t>::extract                   <std::uint16_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator+ (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator- (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator* (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator/ (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator% (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator& (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator| (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator^ (expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator<<(expression<std:: int16_t>,                 expression<std:: int16_t     > const&);
template vra::expression<std:: int16_t> vra::operator>>(expression<std:: int16_t>,                 expression<std:: int16_t     > const&);

template class                                     vra::expression<std::uint16_t>;
template                                           vra::expression<std::uint16_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std::uint16_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std::uint16_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std::uint16_t>::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std::uint16_t>::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std::uint16_t>::expression(     expression<std::uint32_t     > const&);
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::join(std::array<expression<bool         >, 16> const&);
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::join(std::array<expression<std:: int8_t >,  2> const&);
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::join(std::array<expression<std::uint8_t >,  2> const&);
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::join(std::array<expression<std:: int16_t>,  1> const&);
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::join(std::array<expression<std::uint16_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std::uint16_t>::extract                   <bool          , 15>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint16_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint16_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint16_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint16_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint16_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint16_t>::extract                   <std::uint16_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator+ (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator- (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator* (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator/ (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator% (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator& (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator| (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator^ (expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator<<(expression<std::uint16_t>,                 expression<std::uint16_t     > const&);
template vra::expression<std::uint16_t> vra::operator>>(expression<std::uint16_t>,                 expression<std::uint16_t     > const&);

template class                                     vra::expression<std:: int32_t>;
template                                           vra::expression<std:: int32_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std:: int32_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std:: int32_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std:: int32_t>::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std:: int32_t>::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std:: int32_t>::expression(     expression<std::uint32_t     > const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<bool         >, 32> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std:: int8_t >,  4> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std::uint8_t >,  4> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std:: int16_t>,  2> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std::uint16_t>,  2> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std:: int32_t>,  1> const&);
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::join(std::array<expression<std::uint32_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         >            vra::expression<std:: int32_t>::extract                   <bool          , 31>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int32_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int32_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int32_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int32_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int32_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int32_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int32_t>            vra::expression<std:: int32_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t>            vra::expression<std:: int32_t>::extract                   <std::uint32_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator+ (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator- (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator* (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator/ (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator% (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator& (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator| (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator^ (expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator<<(expression<std:: int32_t>,                 expression<std:: int32_t     > const&);
template vra::expression<std:: int32_t> vra::operator>>(expression<std:: int32_t>,                 expression<std:: int32_t     > const&);

template class                                     vra::expression<std::uint32_t>;
template                                           vra::expression<std::uint32_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std::uint32_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std::uint32_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std::uint32_t>::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std::uint32_t>::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std::uint32_t>::expression(     expression<std:: int32_t     > const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<bool         >, 32> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std:: int8_t >,  4> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std::uint8_t >,  4> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std:: int16_t>,  2> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std::uint16_t>,  2> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std:: int32_t>,  1> const&);
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::join(std::array<expression<std::uint32_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         >            vra::expression<std::uint32_t>::extract                   <bool          , 31>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint32_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint32_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint32_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint32_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint32_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint32_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int32_t>            vra::expression<std::uint32_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t>            vra::expression<std::uint32_t>::extract                   <std::uint32_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator+ (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator- (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator* (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator/ (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator% (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator& (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator| (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator^ (expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator<<(expression<std::uint32_t>,                 expression<std::uint32_t     > const&);
template vra::expression<std::uint32_t> vra::operator>>(expression<std::uint32_t>,                 expression<std::uint32_t     > const&);

template class                                     vra::expression<std:: int64_t>;
template                                           vra::expression<std:: int64_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std::uint32_t     > const&);
template                                           vra::expression<std:: int64_t>::expression(     expression<std::uint64_t     > const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<bool         >, 64> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std:: int8_t >,  8> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std::uint8_t >,  8> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std:: int16_t>,  4> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std::uint16_t>,  4> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std:: int32_t>,  2> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std::uint32_t>,  2> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std:: int64_t>,  1> const&);
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::join(std::array<expression<std::uint64_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 31>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 32>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 33>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 34>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 35>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 36>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 37>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 38>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 39>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 40>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 41>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 42>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 43>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 44>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 45>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 46>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 47>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 48>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 49>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 50>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 51>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 52>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 53>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 54>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 55>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 56>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 57>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 58>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 59>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 60>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 61>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 62>() const;
template vra::expression<bool         >            vra::expression<std:: int64_t>::extract                   <bool          , 63>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  4>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  4>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  5>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  5>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  6>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  6>() const;
template vra::expression<std:: int8_t >            vra::expression<std:: int64_t>::extract                   <std:: int8_t  ,  7>() const;
template vra::expression<std::uint8_t >            vra::expression<std:: int64_t>::extract                   <std::uint8_t  ,  7>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  2>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  2>() const;
template vra::expression<std:: int16_t>            vra::expression<std:: int64_t>::extract                   <std:: int16_t ,  3>() const;
template vra::expression<std::uint16_t>            vra::expression<std:: int64_t>::extract                   <std::uint16_t ,  3>() const;
template vra::expression<std:: int32_t>            vra::expression<std:: int64_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t>            vra::expression<std:: int64_t>::extract                   <std::uint32_t ,  0>() const;
template vra::expression<std:: int32_t>            vra::expression<std:: int64_t>::extract                   <std:: int32_t ,  1>() const;
template vra::expression<std::uint32_t>            vra::expression<std:: int64_t>::extract                   <std::uint32_t ,  1>() const;
template vra::expression<std:: int64_t>            vra::expression<std:: int64_t>::extract                   <std:: int64_t ,  0>() const;
template vra::expression<std::uint64_t>            vra::expression<std:: int64_t>::extract                   <std::uint64_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator+ (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator- (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator* (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator/ (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator% (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator& (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator| (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator^ (expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator<<(expression<std:: int64_t>,                 expression<std:: int64_t     > const&);
template vra::expression<std:: int64_t> vra::operator>>(expression<std:: int64_t>,                 expression<std:: int64_t     > const&);

template class                                     vra::expression<std::uint64_t>;
template                                           vra::expression<std::uint64_t>::expression(     expression<bool              > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std:: int8_t      > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std::uint8_t      > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std:: int16_t     > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std::uint16_t     > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std:: int32_t     > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std::uint32_t     > const&);
template                                           vra::expression<std::uint64_t>::expression(     expression<std:: int64_t     > const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<bool         >, 64> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std:: int8_t >,  8> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std::uint8_t >,  8> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std:: int16_t>,  4> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std::uint16_t>,  4> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std:: int32_t>,  2> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std::uint32_t>,  2> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std:: int64_t>,  1> const&);
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::join(std::array<expression<std::uint64_t>,  1> const&);
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  0>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  1>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  2>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  3>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  4>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  5>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  6>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  7>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  8>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          ,  9>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 10>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 11>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 12>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 13>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 14>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 15>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 16>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 17>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 18>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 19>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 20>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 21>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 22>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 23>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 24>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 25>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 26>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 27>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 28>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 29>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 30>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 31>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 32>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 33>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 34>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 35>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 36>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 37>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 38>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 39>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 40>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 41>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 42>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 43>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 44>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 45>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 46>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 47>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 48>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 49>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 50>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 51>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 52>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 53>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 54>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 55>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 56>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 57>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 58>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 59>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 60>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 61>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 62>() const;
template vra::expression<bool         >            vra::expression<std::uint64_t>::extract                   <bool          , 63>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  0>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  0>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  1>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  1>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  2>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  2>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  3>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  3>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  4>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  4>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  5>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  5>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  6>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  6>() const;
template vra::expression<std:: int8_t >            vra::expression<std::uint64_t>::extract                   <std:: int8_t  ,  7>() const;
template vra::expression<std::uint8_t >            vra::expression<std::uint64_t>::extract                   <std::uint8_t  ,  7>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  0>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  0>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  1>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  1>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  2>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  2>() const;
template vra::expression<std:: int16_t>            vra::expression<std::uint64_t>::extract                   <std:: int16_t ,  3>() const;
template vra::expression<std::uint16_t>            vra::expression<std::uint64_t>::extract                   <std::uint16_t ,  3>() const;
template vra::expression<std:: int32_t>            vra::expression<std::uint64_t>::extract                   <std:: int32_t ,  0>() const;
template vra::expression<std::uint32_t>            vra::expression<std::uint64_t>::extract                   <std::uint32_t ,  0>() const;
template vra::expression<std:: int32_t>            vra::expression<std::uint64_t>::extract                   <std:: int32_t ,  1>() const;
template vra::expression<std::uint32_t>            vra::expression<std::uint64_t>::extract                   <std::uint32_t ,  1>() const;
template vra::expression<std:: int64_t>            vra::expression<std::uint64_t>::extract                   <std:: int64_t ,  0>() const;
template vra::expression<std::uint64_t>            vra::expression<std::uint64_t>::extract                   <std::uint64_t ,  0>() const;
template std::ostream&                  vra::operator<<(std::ostream&            ,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator+ (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator- (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator* (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator/ (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator% (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator& (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator| (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator^ (expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator<<(expression<std::uint64_t>,                 expression<std::uint64_t     > const&);
template vra::expression<std::uint64_t> vra::operator>>(expression<std::uint64_t>,                 expression<std::uint64_t     > const&);

template class std::hash<vra::expression<bool         >>;
template class std::hash<vra::expression<std:: int8_t >>;
template class std::hash<vra::expression<std::uint8_t >>;
template class std::hash<vra::expression<std:: int16_t>>;
template class std::hash<vra::expression<std::uint16_t>>;
template class std::hash<vra::expression<std:: int32_t>>;
template class std::hash<vra::expression<std::uint32_t>>;
template class std::hash<vra::expression<std:: int64_t>>;
template class std::hash<vra::expression<std::uint64_t>>;
