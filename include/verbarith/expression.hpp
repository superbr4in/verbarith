#pragma once

#include <verbarith/expression_base.hpp>
#include <verbarith/width.hpp>

namespace vra
{
    template <typename T>
    concept integral_expression_typename =
            std::same_as<T, std::byte>
        || (std::same_as<T, std::remove_cvref_t<T>> && std::integral<T>);

    template <typename T>
    concept expression_typename =
            std::same_as<T, void>
        ||  integral_expression_typename<T>;

    template <expression_typename T = void>
    class expression : public expression_base
    {
        template <expression_typename>
        friend class expression;
        template <integral_expression_typename>
        friend class pointer_expression;

        using expression_base::expression_base;

    public:

        explicit expression(T value) noexcept;
        explicit expression(std::string const& symbol);

        template <integral_expression_typename U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U> const&) noexcept;
        template <integral_expression_typename U>
            requires (widthof(U) > widthof(T))
        explicit expression(expression<U> const&) noexcept;
        template <integral_expression_typename U>
            requires (widthof(U) < widthof(T))
        explicit expression(expression<U> const&) noexcept;

        template <integral_expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;
        template <integral_expression_typename U, std::size_t POSITION>
            requires (widthof(T) >= widthof(U) * (POSITION + 1))
        [[nodiscard]] expression<U> extract() const noexcept;

        [[nodiscard]] T evaluate() const;

        [[nodiscard]] expression<bool> equal(expression const&) const noexcept;
        [[nodiscard]] expression<bool> less(expression const&) const noexcept;

        expression& operator++() noexcept;
        expression& operator--() noexcept;

        expression& operator +=(expression const&) noexcept;
        expression& operator -=(expression const&) noexcept;
        expression& operator *=(expression const&) noexcept;
        expression& operator /=(expression const&) noexcept;
        expression& operator %=(expression const&) noexcept;
        expression& operator &=(expression const&) noexcept;
        expression& operator |=(expression const&) noexcept;
        expression& operator ^=(expression const&) noexcept;
        expression& operator<<=(expression const&) noexcept;
        expression& operator>>=(expression const&) noexcept;

        [[nodiscard]] expression operator-() const noexcept;
        [[nodiscard]] expression operator~() const noexcept;

        [[nodiscard]] expression operator +(expression const&) const noexcept;
        [[nodiscard]] expression operator -(expression const&) const noexcept;
        [[nodiscard]] expression operator *(expression const&) const noexcept;
        [[nodiscard]] expression operator /(expression const&) const noexcept;
        [[nodiscard]] expression operator %(expression const&) const noexcept;
        [[nodiscard]] expression operator &(expression const&) const noexcept;
        [[nodiscard]] expression operator |(expression const&) const noexcept;
        [[nodiscard]] expression operator ^(expression const&) const noexcept;
        [[nodiscard]] expression operator<<(expression const&) const noexcept;
        [[nodiscard]] expression operator>>(expression const&) const noexcept;

    private:

        template <integral_expression_typename U = T, typename Applicator>
        [[nodiscard]] expression<U> derive(Applicator&&) const noexcept;
        template <integral_expression_typename U = T, typename Applicator>
        [[nodiscard]] expression<U> derive(Applicator&&, expression const&) const noexcept;

        template <typename Applicator>
        void update(Applicator&&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&, expression const&) noexcept;

        template <std::size_t INDEX, integral_expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;
    };
    template <>
    class expression<void> : public expression_base
    { };
}

namespace std
{
    template <vra::expression_typename T>
    struct hash<vra::expression<T>>
    {
        [[nodiscard]] std::size_t operator()(vra::expression<T> const&) const noexcept;
    };
}
