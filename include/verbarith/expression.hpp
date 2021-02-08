#pragma once

#include <climits>
#include <functional>
#include <ostream>

#define widthof(T) (std::is_same_v<T, bool> ? 1 : sizeof(T) * CHAR_BIT)

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;
// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_context;

namespace vra
{
    template <typename T>
    concept expression_typename = std::is_integral_v<T> && std::is_same_v<T, std::decay_t<T>>;
    template <typename F, typename... Ts>
    concept expression_maker =
        requires (F&& f, _Z3_context* const context, Ts&&... ts)
        {
            { std::invoke(std::forward<F>(f), context, std::forward<Ts>(ts)...) } -> std::same_as<_Z3_ast*>;
        };

    template <expression_typename T>
    class expression;

    template <expression_typename T>
    std::ostream& operator<<(std::ostream&, expression<T> const&);

    template <expression_typename T>
    class expression
    {
        template <expression_typename>
        friend class expression;

        friend std::hash<expression>;

        _Z3_ast* base_;

        template <typename... Arguments, expression_maker<Arguments...> Maker>
        explicit expression(Maker&&, Arguments&&...);

        template <typename... Arguments, expression_maker<_Z3_ast*, Arguments...> Maker>
        void apply(Maker&&, Arguments&&...);
        void simplify();

    public:

        explicit expression(T value);
        explicit expression(std::string const& symbol);

        template <expression_typename U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U> const&);
        template <expression_typename U>
            requires (widthof(U) > widthof(T))
        explicit expression(expression<U> const&);
        template <expression_typename U>
            requires (widthof(U) < widthof(T))
        explicit expression(expression<U> const&);

        ~expression();

        expression(expression const&);
        expression& operator=(expression const&);

        expression(expression&&) noexcept;
        expression& operator=(expression&&) noexcept;

        [[nodiscard]] bool is_conclusive() const;
        [[nodiscard]] T evaluate() const;

        [[nodiscard]] bool operator==(expression const&) const;
        [[nodiscard]] bool operator!=(expression const&) const;

        template <expression_typename U, std::size_t POSITION>
            requires (widthof(T) >= widthof(U) * (POSITION + 1))
        [[nodiscard]] expression<U> extract() const;

        [[nodiscard]] expression<bool> equal(expression const&) const;
        [[nodiscard]] expression<bool> less_than(expression const&) const;

        [[nodiscard]] expression operator-() const;
        [[nodiscard]] expression operator~() const;

        expression& operator++();
        expression& operator--();

        expression& operator+=(expression const&);
        expression& operator-=(expression const&);
        expression& operator*=(expression const&);
        expression& operator/=(expression const&);
        expression& operator%=(expression const&);

        expression& operator&=(expression const&);
        expression& operator|=(expression const&);
        expression& operator^=(expression const&);

        expression& operator<<=(expression const&);
        expression& operator>>=(expression const&);

        friend std::ostream& operator<< <>(std::ostream&, expression const&);

        template <expression_typename U>
            requires (widthof(U) <= widthof(T))
        static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&);

    private:

        template <std::size_t INDEX, expression_typename U>
            requires (widthof(U) <= widthof(T))
        static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&);
    };

    template <expression_typename T>
    expression<T> operator+(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator-(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator*(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator/(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator%(expression<T>, expression<T> const&);

    template <expression_typename T>
    expression<T> operator&(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator|(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator^(expression<T>, expression<T> const&);

    template <expression_typename T>
    expression<T> operator<<(expression<T>, expression<T> const&);
    template <expression_typename T>
    expression<T> operator>>(expression<T>, expression<T> const&);
}

namespace std
{
    template <vra::expression_typename T>
    struct hash<vra::expression<T>>
    {
        std::size_t operator()(vra::expression<T> const& expression) const noexcept;
    };
}
