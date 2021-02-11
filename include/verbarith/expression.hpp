#pragma once

#include <climits>

#include <verbarith/expression_base.hpp>

#define widthof(T) (std::same_as<std::remove_cvref_t<T>, bool> ? 1 : sizeof(T) * CHAR_BIT)

struct _Z3_ast; // NOLINT [cert-dcl51-cpp]

namespace vra
{
    template <typename T>
    concept expression_typename = std::same_as<T, void> || (std::integral<T> && std::same_as<T, std::remove_cvref_t<T>>);

    template <expression_typename T = void>
    class expression;

    template <expression_typename T>
    bool operator==(expression<T> const&, expression<T> const&) noexcept;
    template <expression_typename T>
    std::ostream& operator<<(std::ostream&, expression<T> const&) noexcept;

    template <>
    class expression<> : expression_base<_Z3_ast>
    {
        template <expression_typename>
        friend class expression;
        friend std::hash<expression>;

        using expression_base::expression_base;

    public:

        friend bool operator== <>(expression const&, expression const&) noexcept;
        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;
    };
    template <expression_typename T>
    class expression : expression<>
    {
        template <expression_typename>
        friend class expression;
        friend std::hash<expression>;

        using expression<>::expression;

    public:

        explicit expression(T value) noexcept;
        explicit expression(std::string const& symbol);

        template <expression_typename U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U> const&) noexcept;
        explicit expression(expression<> const&) noexcept;
        template <expression_typename U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U>&&) noexcept;
        explicit expression(expression<>&&) noexcept;

        template <expression_typename U>
            requires (widthof(U) > widthof(T))
        explicit expression(expression<U> const&) noexcept;
        template <expression_typename U>
            requires (widthof(U) < widthof(T))
        explicit expression(expression<U> const&) noexcept;

        [[nodiscard]] unsigned arity() const;

        [[nodiscard]] bool is_conclusive() const noexcept;
        [[nodiscard]] T evaluate() const;

        template <expression_typename U, std::size_t POSITION>
            requires (widthof(T) >= widthof(U) * (POSITION + 1))
        [[nodiscard]] expression<U> extract() const noexcept;

        template <expression_typename U>
            requires (!std::same_as<U, void>)
        [[nodiscard]] expression<U> dereference() const noexcept;

        [[nodiscard]] expression<bool> equal(expression const&) const noexcept;
        [[nodiscard]] expression<bool> less(expression const&) const noexcept;

        [[nodiscard]] expression operator-() const noexcept;
        [[nodiscard]] expression operator~() const noexcept;

        expression& operator++() noexcept;
        expression& operator--() noexcept;

        expression& operator+=(expression const&) noexcept;
        expression& operator-=(expression const&) noexcept;
        expression& operator*=(expression const&) noexcept;
        expression& operator/=(expression const&) noexcept;
        expression& operator%=(expression const&) noexcept;

        expression& operator&=(expression const&) noexcept;
        expression& operator|=(expression const&) noexcept;
        expression& operator^=(expression const&) noexcept;

        expression& operator<<=(expression const&) noexcept;
        expression& operator>>=(expression const&) noexcept;

        friend bool operator== <>(expression const&, expression const&) noexcept;
        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;

        template <expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;

    private:

        template <std::size_t INDEX, expression_typename U>
            requires (widthof(U) <= widthof(T))
        [[nodiscard]] static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&) noexcept;
    };

    template <expression_typename T>
    expression<T> operator+(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator-(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator*(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator/(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator%(expression<T>, expression<T> const&) noexcept;

    template <expression_typename T>
    expression<T> operator&(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator|(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator^(expression<T>, expression<T> const&) noexcept;

    template <expression_typename T>
    expression<T> operator<<(expression<T>, expression<T> const&) noexcept;
    template <expression_typename T>
    expression<T> operator>>(expression<T>, expression<T> const&) noexcept;
}

namespace std
{
    template <vra::expression_typename T>
    struct hash<vra::expression<T>>
    {
        [[nodiscard]] std::size_t operator()(vra::expression<T> const&) const noexcept;
    };
}
