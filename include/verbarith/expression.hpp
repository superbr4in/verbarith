#pragma once

#include <stimpak/binary.hpp>
#include <stimpak/conceptual.hpp>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;
// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_context;

namespace vra
{
    template <sti::decayed_integral T>
    class expression;

    template <sti::decayed_integral T>
    std::ostream& operator<<(std::ostream&, expression<T> const&);

    template <sti::decayed_integral T>
    class expression
    {
        friend std::hash<expression>;

        template <sti::decayed_integral> friend class expression;

        _Z3_ast* base_;

        template <sti::decayed... Arguments>
        explicit expression(_Z3_ast* (*)(_Z3_context*, Arguments...), Arguments...);

        template <sti::decayed... Arguments>
        void apply(_Z3_ast* (*)(_Z3_context*, _Z3_ast*, Arguments...), Arguments...);
        void simplify();

    public:

        explicit expression(T value);
        explicit expression(std::string const& symbol);

        template <sti::decayed_integral U>
            requires (widthof(U) == widthof(T))
        explicit expression(expression<U> const&);
        template <sti::decayed_integral U>
            requires (widthof(U) > widthof(T))
        explicit expression(expression<U> const&);
        template <sti::decayed_integral U>
            requires (widthof(U) < widthof(T))
        explicit expression(expression<U> const&);

        ~expression();

        expression(expression const&);
        expression& operator=(expression const&);

        expression(expression&&) noexcept;
        expression& operator=(expression&&) noexcept;

        template <sti::decayed_integral U>
            requires (widthof(U) <= widthof(T))
        static expression join(std::array<expression<U>, widthof(T) / widthof(U)> const&);

        template <sti::decayed_integral U, std::size_t POSITION>
            requires (widthof(T) >= widthof(U) * (POSITION + 1))
        [[nodiscard]] expression<U> extract() const;

        [[nodiscard]] bool is_conclusive() const;
        [[nodiscard]] T evaluate() const;

        [[nodiscard]] bool operator==(expression const&) const;
        [[nodiscard]] bool operator!=(expression const&) const;

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
    };

    template <sti::decayed_integral T>
    expression<T> operator+(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator-(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator*(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator/(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator%(expression<T>, expression<T> const&);

    template <sti::decayed_integral T>
    expression<T> operator&(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator|(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator^(expression<T>, expression<T> const&);

    template <sti::decayed_integral T>
    expression<T> operator<<(expression<T>, expression<T> const&);
    template <sti::decayed_integral T>
    expression<T> operator>>(expression<T>, expression<T> const&);
}

namespace std
{
    template <sti::decayed_integral T>
    struct hash<vra::expression<T>>
    {
        std::size_t operator()(vra::expression<T> const& expression) const noexcept;
    };
}
