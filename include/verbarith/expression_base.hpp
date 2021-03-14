#pragma once

#include <memory>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_ast;

namespace vra
{
    template <typename>
    class resource_handler;

    class expression_base
    {
        friend struct std::hash<expression_base>;

        std::unique_ptr<resource_handler<_Z3_ast>> base_;

    protected:

        explicit expression_base(_Z3_ast*) noexcept;

        ~expression_base() noexcept;

    public:

        expression_base(expression_base const&) noexcept;
        expression_base& operator=(expression_base const&) noexcept;

        expression_base(expression_base&&) noexcept;
        expression_base& operator=(expression_base&&) noexcept;

        [[nodiscard]] std::size_t width() const noexcept;

        [[nodiscard]] bool conclusive() const noexcept;
        [[nodiscard]] bool operator==(expression_base const&) const noexcept;

        void substitute(std::string const& symbol, expression_base const& value) noexcept;

        friend std::ostream& operator<<(std::ostream&, expression_base const&) noexcept;

    protected:

        [[nodiscard]] _Z3_ast* base() const noexcept;
        void base(_Z3_ast*) noexcept;
    };
}

namespace std
{
    template <>
    struct hash<vra::expression_base>
    {
        [[nodiscard]] std::size_t operator()(vra::expression_base const&) const noexcept;
    };
}
