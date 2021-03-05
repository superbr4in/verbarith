#pragma once

#include <verbarith/expression_base.hpp>
#include <verbarith/width.hpp>

namespace vra
{
    template <typename T>
    concept expression_typename =
            std::same_as<T, std::remove_cvref_t<T>>
        && (    std::same_as<std::remove_pointer_t<T>, void>
            || (std::same_as<std::remove_pointer_t<T>, std::remove_cvref_t<std::remove_pointer_t<T>>> && std::integral<std::remove_pointer_t<T>>));

    template <expression_typename T = void>
    class expression;

    template <>
    class expression<> : public expression_base
    {
        using expression_base::expression_base;
    };
    template <expression_typename T>
    class expression : public expression<>
    {
        template <expression_typename>
        friend class expression;

        using expression<>::expression;

    public:

        explicit expression(T value) noexcept;
        explicit expression(std::string const& symbol);

        template <expression_typename U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U> const&) noexcept;
        template <expression_typename U>
            requires (widthof(U) > widthof(T))
        explicit expression(expression<U> const&) noexcept;
        template <expression_typename U>
            requires (widthof(U) < widthof(T))
        explicit expression(expression<U> const&) noexcept;

        template <expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;
        template <expression_typename U, std::size_t POSITION>
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

        [[nodiscard]] expression operator +(expression const&) noexcept;
        [[nodiscard]] expression operator -(expression const&) noexcept;
        [[nodiscard]] expression operator *(expression const&) noexcept;
        [[nodiscard]] expression operator /(expression const&) noexcept;
        [[nodiscard]] expression operator %(expression const&) noexcept;
        [[nodiscard]] expression operator &(expression const&) noexcept;
        [[nodiscard]] expression operator |(expression const&) noexcept;
        [[nodiscard]] expression operator ^(expression const&) noexcept;
        [[nodiscard]] expression operator<<(expression const&) noexcept;
        [[nodiscard]] expression operator>>(expression const&) noexcept;

    private:

        template <expression_typename U = T, typename Applicator>
        [[nodiscard]] expression<U> derive(Applicator&&) const noexcept;
        template <expression_typename U = T, typename Applicator>
        [[nodiscard]] expression<U> derive(Applicator&&, expression const&) const noexcept;

        template <typename Applicator>
        void update(Applicator&&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&, expression const&) noexcept;

        template <std::size_t INDEX, expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;
    };

    template <>
    class expression<void*> : public expression_base
    {
        using expression_base::expression_base;
    };
    template <expression_typename T>
    class expression<T*> : public expression<void*>
    {
        using expression<void*>::expression;

    public:

        explicit expression(std::uintptr_t value) noexcept;

        [[nodiscard]] expression<T> dereference() const noexcept;
    };
}

namespace std
{
    template <vra::expression_typename T>
    struct hash<vra::expression<T>>
    {
        [[nodiscard]] std::size_t operator()(vra::expression<T> const&) const noexcept;
    };
}
