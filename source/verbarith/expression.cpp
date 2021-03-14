#include <algorithm>

#include <verbarith/expression.hpp>
#include <verbarith/expression_sort.ipp>
#include <verbarith/preprocessor_types.hpp>

namespace vra
{
    template <expression_typename T>
    static expression<T> const zero(0);
    template <expression_typename T>
    static expression<T> const one(1);

    template <expression_typename T>
    expression<T>::expression(T const value) noexcept :
        expression(resource_context::apply(Z3_mk_unsigned_int64, static_cast<std::uint64_t>(value), expression_sort::instance<widthof(T)>()))
    { }
    template <expression_typename T>
    expression<T>::expression(std::string const& symbol) :
        expression(resource_context::apply(Z3_mk_const, resource_context::apply(Z3_mk_string_symbol, symbol.c_str()), expression_sort::instance<widthof(T)>()))
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");
    }

    template <expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(other.base())
    { }
    template <expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) > widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(resource_context::apply(Z3_mk_extract, unsigned{widthof(T) - 1}, unsigned{0}, other.base()))
    {
        update(Z3_simplify);
    }
    template <expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) < widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(resource_context::apply(Z3_mk_zero_ext, unsigned{widthof(T) - widthof(U)}, other.base()))
    {
        update(Z3_simplify);
    }

    template <expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts) noexcept
    {
        if constexpr (widthof(T) == widthof(U))
        {
            return expression<T>(std::get<0>(parts));
        }
        else
        {
            expression<T> result(join<0>(parts));
            result.update(Z3_simplify);

            return result;
        }
    }
    template <expression_typename T>
    template <integral_expression_typename U, std::size_t POSITION>
        requires (widthof(T) >= widthof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const noexcept
    {
        expression<U> result(resource_context::apply(Z3_mk_extract, unsigned{(widthof(U) * (POSITION + 1)) - 1}, unsigned{widthof(U) * POSITION}, base()));
        result.update(Z3_simplify);

        return result;
    }

    template <expression_typename T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; resource_context::apply(Z3_get_numeral_uint64, base(), &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <expression_typename T>
    expression<bool> expression<T>::equal(expression const& other) const noexcept
    {
        auto result = derive<bool>(Z3_mk_eq, other);
        result.update(Z3_mk_ite, one<bool>, zero<bool>);
        result.update(Z3_simplify);

        return result;
    }
    template <expression_typename T>
    expression<bool> expression<T>::less(expression const& other) const noexcept
    {
        auto result = derive<bool>(std::signed_integral<T> ? Z3_mk_bvslt : Z3_mk_bvult, other);
        result.update(Z3_mk_ite, one<bool>, zero<bool>);
        result.update(Z3_simplify);

        return result;
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator++() noexcept
    {
        return operator+=(one<T>);
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator--() noexcept
    {
        return operator-=(one<T>);
    }

    template <expression_typename T>
    expression<T>& expression<T>::operator +=(expression const& other) noexcept
    {
        update(Z3_mk_bvadd, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator -=(expression const& other) noexcept
    {
        update(Z3_mk_bvsub, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator *=(expression const& other) noexcept
    {
        update(Z3_mk_bvmul, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator /=(expression const& other) noexcept
    {
        update(std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator %=(expression const& other) noexcept
    {
        update(std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator &=(expression const& other) noexcept
    {
        update(Z3_mk_bvand, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator |=(expression const& other) noexcept
    {
        update(Z3_mk_bvor, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator ^=(expression const& other) noexcept
    {
        update(Z3_mk_bvxor, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator<<=(expression const& other) noexcept
    {
        update(Z3_mk_bvshl, other);
        update(Z3_simplify);

        return *this;
    }
    template <expression_typename T>
    expression<T>& expression<T>::operator>>=(expression const& other) noexcept
    {
        update(Z3_mk_bvlshr, other);
        update(Z3_simplify);

        return *this;
    }

    template <expression_typename T>
    expression<T> expression<T>::operator-() const noexcept
    {
        return derive(Z3_mk_bvneg);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator~() const noexcept
    {
        return derive(Z3_mk_bvnot);
    }

    template <expression_typename T>
    expression<T> expression<T>::operator +(expression const& other) noexcept
    {
        return derive(Z3_mk_bvadd, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator -(expression const& other) noexcept
    {
        return derive(Z3_mk_bvsub, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator *(expression const& other) noexcept
    {
        return derive(Z3_mk_bvmul, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator /(expression const& other) noexcept
    {
        return derive(std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator %(expression const& other) noexcept
    {
        return derive(std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator &(expression const& other) noexcept
    {
        return derive(Z3_mk_bvand, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator |(expression const& other) noexcept
    {
        return derive(Z3_mk_bvor, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator ^(expression const& other) noexcept
    {
        return derive(Z3_mk_bvxor, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator<<(expression const& other) noexcept
    {
        return derive(Z3_mk_bvshl, other);
    }
    template <expression_typename T>
    expression<T> expression<T>::operator>>(expression const& other) noexcept
    {
        return derive(Z3_mk_bvlshr, other);
    }

    template <expression_typename T>
    template <integral_expression_typename U, typename Applicator>
    expression<U> expression<T>::derive(Applicator&& applicator) const noexcept
    {
        expression<U> derived(resource_context::apply(std::forward<Applicator>(applicator), base()));
        derived.update(Z3_simplify);

        return derived;
    }
    template <expression_typename T>
    template <integral_expression_typename U, typename Applicator>
    expression<U> expression<T>::derive(Applicator&& applicator, expression const& other) const noexcept
    {
        expression<U> derived(resource_context::apply(std::forward<Applicator>(applicator), base(), other.base()));
        derived.update(Z3_simplify);

        return derived;
    }

    template <expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base()));
    }
    template <expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator, expression const& other) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base(), other.base()));
    }
    template <expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator, expression const& other_1, expression const& other_2) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base(), other_1.base(), other_2.base()));
    }

    template <expression_typename T>
    template <std::size_t INDEX, integral_expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts) noexcept
    {
        if constexpr (INDEX == widthof(T) / widthof(U) - 2)
        {
            return expression(resource_context::apply(Z3_mk_concat, std::get<INDEX + 1>(parts).base(), std::get<INDEX>(parts).base()));
        }
        else
        {
            return expression(resource_context::apply(Z3_mk_concat, join<INDEX + 1>(parts).base(), std::get<INDEX>(parts).base()));
        }
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    template <vra::expression_typename T>
    std::size_t hash<vra::expression<T>>::operator()(vra::expression<T> const& value) const noexcept
    {
        static hash<vra::expression_base> constexpr base_hasher;
        return base_hasher(value);
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

#define INSTANTIATE_EXPRESSION_SQUARE_INDEXED(T, U, index)\
    template vra::EXPRESSION(U) vra::EXPRESSION(T)::extract<TYPE(U), index>() const
#define INSTANTIATE_EXPRESSION_SQUARE(T, U)\
    IF_NOT_EQUAL(            T, U, template                    vra::EXPRESSION(T)::expression(     EXPRESSION(U) const&))                          ;\
    IF_TYPE_WIDTH_DIVIDABLE( T, U, template vra::EXPRESSION(T) vra::EXPRESSION(T)::join(std::array<EXPRESSION(U), TYPE_WIDTH_DIVIDE(T, U)> const&));\
    LOOP_TYPE_WIDTH_DIVIDE_2(T, U, INSTANTIATE_EXPRESSION_SQUARE_INDEXED, T, U)
#define INSTANTIATE_EXPRESSION(T)\
    template class           vra::EXPRESSION(T) ;\
    template class std::hash<vra::EXPRESSION(T)>;\
    LOOP_TYPES_1(INSTANTIATE_EXPRESSION_SQUARE, T)
LOOP_TYPES_0(INSTANTIATE_EXPRESSION);
