#pragma once

#include <memory>
#include <unordered_set>

#include <verbarith/width.hpp>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;

namespace vra
{
    template <typename T>
    concept integral_expression_typename =
            std::same_as<T, std::byte>
        || (std::same_as<T, std::remove_cvref_t<T>> && !std::same_as<T, bool> && std::integral<T>);

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
    template <typename T>
    std::wostream& operator<<(std::wostream&, expression<T> const&) noexcept;

    template <>
    class expression<>
    {
        template <typename, typename>
        friend class expression;
        friend struct std::hash<expression>;

        friend bool operator== <>(expression const&, expression const&) noexcept;

        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;
        friend std::wostream& operator<< <>(std::wostream&, expression const&) noexcept;

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
        [[nodiscard]] expression<T> const& as_expression() const;
        template <integral_expression_typename T>
        expression<T>& as_expression();

        [[nodiscard]] bool conclusive() const noexcept;

        template <integral_expression_typename T>
        [[nodiscard]] T evaluate() const;

        [[nodiscard]] std::unordered_set<std::string> dependencies() const noexcept;
        [[nodiscard]] std::unordered_set<expression> dependencies_indirect() const noexcept;

        void substitute(std::string const& key_symbol, expression const& value) noexcept;
        void substitute_indirect(expression const& key_pointer, expression<std::byte> const& value) noexcept;

    private:

        [[nodiscard]] _Z3_ast* base() const noexcept;
        void base(_Z3_ast*) noexcept;

        [[nodiscard]] std::size_t width() const noexcept;

        [[nodiscard]] std::string representation() const noexcept;

        template <typename T, typename Applicator>
        [[nodiscard]] expression<T> derive(Applicator&&) const noexcept;
        template <typename T, typename U, typename Applicator>
        [[nodiscard]] expression<T> derive(Applicator&&, expression<U> const&) const noexcept;

        template <typename Applicator>
        void update(Applicator&&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&) noexcept;
        template <typename Applicator>
        void update(Applicator&&, expression const&, expression const&) noexcept;
    };

    template <>
    class expression<bool> : public expression<>
    {
        template <typename, typename>
        friend class expression;

        friend bool operator== <>(expression const&, expression const&) noexcept;

        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;
        friend std::wostream& operator<< <>(std::wostream&, expression const&) noexcept;

        explicit expression(_Z3_ast*) noexcept;

    public:

        static expression const unsatisfiable;
        static expression const valid;

        [[nodiscard]] static expression symbol(std::string const& symbol);

        template <integral_expression_typename T>
        explicit expression(expression<T> const&);

        [[nodiscard]] bool evaluate() const;

        [[nodiscard]] expression<bool> equal(expression const&) const noexcept;

        expression& operator&=(expression const&) noexcept;
        expression& operator|=(expression const&) noexcept;
        expression& operator^=(expression const&) noexcept;

        [[nodiscard]] expression operator!() const noexcept;

        [[nodiscard]] expression operator&(expression const&) const noexcept;
        [[nodiscard]] expression operator|(expression const&) const noexcept;
        [[nodiscard]] expression operator^(expression const&) const noexcept;
    };

    template <integral_expression_typename T>
    class expression<T> : public expression<>
    {
        template <typename, typename>
        friend class expression;

        friend bool operator== <>(expression const&, expression const&) noexcept;

        friend std::ostream& operator<< <>(std::ostream&, expression const&) noexcept;
        friend std::wostream& operator<< <>(std::wostream&, expression const&) noexcept;

        template <typename = void>
        explicit expression(_Z3_ast*) noexcept;

    public:

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        expression(T value) noexcept;

        [[nodiscard]] static expression symbol(std::string const& symbol);

        explicit expression(expression<bool> const&) noexcept;

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
        [[nodiscard]] size_t operator()(vra::expression<> const&) const noexcept;
    };

    template <>
    struct hash<vra::expression<bool>>
    {
        [[nodiscard]] size_t operator()(vra::expression<bool> const&) const noexcept;
    };

    template <vra::integral_expression_typename T>
    struct hash<vra::expression<T>>
    {
        [[nodiscard]] size_t operator()(vra::expression<T> const&) const noexcept;
    };
}
