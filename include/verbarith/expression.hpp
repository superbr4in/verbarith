#pragma once

#include <memory>

#include <verbarith/width.hpp>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;

namespace vra
{
    template <typename T>
    concept integral_expression_typename =
            std::same_as<T, std::byte>
        || (std::same_as<T, std::remove_cvref_t<T>> && std::integral<T>);

    template <typename>
    class resource_handler;

    template <typename = void,
        // Enables partial specializations with type constraints
        typename = void>
    class expression;

    template <typename T>
    bool operator==(expression<T> const&, expression<T> const&) noexcept;
    template <typename T>
    std::ostream& operator<<(std::ostream&, expression<T> const&) noexcept;

    template <>
    class expression<>
    {
        template <typename, typename>
        friend class expression;
        friend struct std::hash<expression>;

        friend bool operator== <>(expression const&, expression const&) noexcept;
        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;

        std::unique_ptr<resource_handler<_Z3_ast>> base_;

        explicit expression(_Z3_ast*) noexcept;

    public:

        virtual ~expression() noexcept;

        template <integral_expression_typename T>
        explicit expression(expression<T> const&) noexcept;
        expression(expression const&) noexcept;
        template <integral_expression_typename T>
        expression& operator=(expression<T> const&);
        expression& operator=(expression const&) noexcept;

        template <integral_expression_typename T>
        explicit expression(expression<T>&&) noexcept;
        expression(expression&&) noexcept;
        template <integral_expression_typename T>
        expression& operator=(expression<T>&&);
        expression& operator=(expression&&) noexcept;

        template <integral_expression_typename T>
        [[nodiscard]] expression<T> const& as() const;
        template <integral_expression_typename T>
        expression<T>& as();

        [[nodiscard]] bool conclusive() const noexcept;

        void substitute(expression const& key, expression const& value);
        void substitute(std::string const& key_symbol, expression const& value);

    private:

        [[nodiscard]] _Z3_ast* base() const noexcept;
        void base(_Z3_ast*) noexcept;

        [[nodiscard]] std::size_t width() const noexcept;
    };

    template <integral_expression_typename T>
    class expression<T> : public expression<>
    {
        template <typename, typename>
        friend class expression;

        friend bool operator== <>(expression const&, expression const&) noexcept;
        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;

        template <typename = void>
        explicit expression(_Z3_ast*) noexcept;

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

        template <integral_expression_typename U>
            requires (widthof(U) >= widthof(std::byte))
        [[nodiscard]] expression<U> dereference() const noexcept;

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

        template <integral_expression_typename U, std::size_t COUNT, typename Generator>
            requires (COUNT > 1)
        [[nodiscard]] static expression<U> concatenate(Generator const&) noexcept;
    };

    template <typename T>
    expression(T) -> expression<T>;
}

namespace std
{
    template <>
    struct hash<vra::expression<>>
    {
        [[nodiscard]] std::size_t operator()(vra::expression<> const&) const noexcept;
    };

    template <vra::integral_expression_typename T>
    struct hash<vra::expression<T>>
    {
        [[nodiscard]] std::size_t operator()(vra::expression<T> const&) const noexcept;
    };
}
