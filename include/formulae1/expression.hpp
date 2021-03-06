#pragma once

#include <concepts>
#include <memory>
#include <unordered_set>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;
// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_context;

extern "C"
{
    void Z3_inc_ref(_Z3_context*, _Z3_ast*);
    void Z3_dec_ref(_Z3_context*, _Z3_ast*);
}

namespace fml
{
    // clang-format off
    template <typename T>
    concept integral_expression_typename =
            std::same_as<T, std::byte>
        || (std::same_as<T, std::remove_cvref_t<T>> && !std::same_as<T, bool> && std::is_integral_v<T>);
    // clang-format on

    template <typename, typename ResourceBase, void(_Z3_context*, ResourceBase*), void(_Z3_context*, ResourceBase*)>
    class z3_resource;
    using z3_ast = z3_resource<_Z3_ast, _Z3_ast, Z3_inc_ref, Z3_dec_ref>;

    template <typename = void,
        // Enables partial specializations with type constraints
        typename = void>
    class expression;

    template <typename T = void>
    [[nodiscard]] expression<T> parse_expression(std::string const&);

    template <typename T>
    std::ostream& operator<<(std::ostream&, expression<T> const&) noexcept;
    template <typename T>
    std::wostream& operator<<(std::wostream&, expression<T> const&) noexcept;

    template <>
    class expression<>
    {
        template <typename, typename>
        friend class expression;
        friend class expression_model;
        friend class expression_solver;

        friend struct std::hash<expression>;

        friend expression parse_expression<>(std::string const&);

        std::unique_ptr<z3_ast> base_;

        explicit expression(z3_ast) noexcept;

    public:
        virtual ~expression() noexcept;

        template <integral_expression_typename T>
        explicit expression(expression<T> const&) noexcept;
        expression(expression const&) noexcept;
        template <integral_expression_typename T>
        expression& operator=(expression<T> const&) noexcept;
        expression& operator=(expression const&) noexcept;

        template <integral_expression_typename T>
        explicit expression(expression<T>&&) noexcept;
        expression(expression&&) noexcept;
        template <integral_expression_typename T>
        expression& operator=(expression<T>&&) noexcept;
        expression& operator=(expression&&) noexcept;

        template <integral_expression_typename T>
        [[nodiscard]] expression<T> const& as_expression() const;
        template <integral_expression_typename T>
        expression<T>& as_expression();

        [[nodiscard]] bool operator==(expression const&) const noexcept;

        [[nodiscard]] bool conclusive() const noexcept;

        [[nodiscard]] std::string representation() const noexcept;

        template <integral_expression_typename T>
        [[nodiscard]] T evaluate() const;

        [[nodiscard]] std::unordered_set<std::string> dependencies() const noexcept;
        [[nodiscard]] std::unordered_set<expression> dependencies_indirect() const noexcept;

        void substitute(std::string const& key_symbol, expression const& value) noexcept;
        void substitute_indirect(expression const& key_pointer, expression<std::byte> const& value) noexcept;

    private:
        [[nodiscard]] std::size_t size() const noexcept;
    };

    template <>
    class expression<bool> : public expression<>
    {
        template <typename, typename>
        friend class expression;
        friend class expression_model;

        friend expression parse_expression<>(std::string const&);

        explicit expression(z3_ast) noexcept;

    public:
        explicit expression(bool) noexcept;

        [[nodiscard]] static expression symbol(std::string const& symbol);

        template <integral_expression_typename T>
        explicit expression(expression<T> const&);

        [[nodiscard]] bool evaluate() const;

        void reduce();

        [[nodiscard]] expression operator!() const noexcept;

        [[nodiscard]] expression equals(expression const&) const noexcept;
        [[nodiscard]] expression implies(expression const&) const noexcept;

        expression& operator&=(expression const&) noexcept;
        expression& operator|=(expression const&) noexcept;
        expression& operator^=(expression const&) noexcept;
    };

    [[nodiscard]] expression<bool> operator&(expression<bool>, expression<bool> const&) noexcept;
    [[nodiscard]] expression<bool> operator|(expression<bool>, expression<bool> const&) noexcept;
    [[nodiscard]] expression<bool> operator^(expression<bool>, expression<bool> const&) noexcept;

    template <integral_expression_typename T>
    class expression<T> : public expression<>
    {
        template <typename, typename>
        friend class expression;
        friend class expression_model;

        friend expression parse_expression<>(std::string const&);

        explicit expression(z3_ast) noexcept;

    public:
        explicit expression(T) noexcept;

        [[nodiscard]] static expression symbol(std::string const& symbol);

        explicit expression(expression<bool> const&) noexcept;

        template <integral_expression_typename U>
        explicit expression(expression<U> const&) noexcept requires(sizeof(U) == sizeof(T));
        template <integral_expression_typename U>
        explicit expression(expression<U> const&) noexcept requires(sizeof(U) > sizeof(T));
        template <integral_expression_typename U>
        explicit expression(expression<U> const&) noexcept requires(sizeof(U) < sizeof(T));

        template <integral_expression_typename U>
        [[nodiscard]] static expression join(std::array<expression<U>, sizeof(T) / sizeof(U)> const&) noexcept requires(sizeof(U) <= sizeof(T));
        template <integral_expression_typename U, std::size_t POSITION>
        [[nodiscard]] expression<U> extract() const noexcept requires(sizeof(T) >= sizeof(U) * (POSITION + 1));

        [[nodiscard]] T evaluate() const;

        template <integral_expression_typename U>
        [[nodiscard]] expression<U> dereference() const noexcept;

        [[nodiscard]] expression operator-() const noexcept;
        [[nodiscard]] expression operator~() const noexcept;

        [[nodiscard]] expression<bool> equals(expression const&) const noexcept;
        [[nodiscard]] expression<bool> less_than(expression const&) const noexcept;

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

    private:
        template <integral_expression_typename U, std::size_t COUNT, typename Generator>
        [[nodiscard]] static expression<U> concatenate(Generator const&) noexcept requires(COUNT > 1);
    };

    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator+(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator-(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator*(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator/(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator%(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator&(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator|(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator^(expression<T>, expression<T> const&) noexcept;

    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator<<(expression<T>, expression<T> const&) noexcept;
    template <integral_expression_typename T>
    [[nodiscard]] expression<T> operator>>(expression<T>, expression<T> const&) noexcept;

    template <typename T>
    expression(T) -> expression<T>;
}

fml::expression<std::int8_t> operator""_e8(unsigned long long int);
fml::expression<std::int16_t> operator""_e16(unsigned long long int);
fml::expression<std::int32_t> operator""_e32(unsigned long long int);
fml::expression<std::int64_t> operator""_e64(unsigned long long int);
fml::expression<std::uint8_t> operator""_eU8(unsigned long long int);
fml::expression<std::uint16_t> operator""_eU16(unsigned long long int);
fml::expression<std::uint32_t> operator""_eU32(unsigned long long int);
fml::expression<std::uint64_t> operator""_eU64(unsigned long long int);

namespace std
{
    template <>
    struct hash<fml::expression<>>
    {
        [[nodiscard]] size_t operator()(fml::expression<> const&) const noexcept;
    };

    template <>
    struct hash<fml::expression<bool>>
    {
        [[nodiscard]] size_t operator()(fml::expression<bool> const&) const noexcept;
    };

    template <fml::integral_expression_typename T>
    struct hash<fml::expression<T>>
    {
        [[nodiscard]] size_t operator()(fml::expression<T> const&) const noexcept;
    };
}
