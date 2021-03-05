#pragma once

#include <verbarith/resource_handler.hpp>

struct _Z3_ast; // NOLINT [cert-dcl51-cpp]

namespace vra
{
    class expression_base : public resource_handler<_Z3_ast>
    {
        friend std::hash<expression_base>;

        using resource_handler::resource_handler;

    protected:

        virtual ~expression_base() noexcept = 0;

        expression_base(expression_base const&) noexcept = default;
        expression_base& operator=(expression_base const&) noexcept = default;

        expression_base(expression_base&&) noexcept = default;
        expression_base& operator=(expression_base&&) noexcept = default;

    public:

        [[nodiscard]] bool conclusive() const noexcept;
        [[nodiscard]] bool operator==(expression_base const&) const noexcept;

        friend std::ostream& operator<<(std::ostream&, expression_base const&) noexcept;
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
