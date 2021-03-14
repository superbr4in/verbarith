#pragma once

#include <z3.h>

#include <verbarith/resource_handler.hpp>

namespace vra
{
    class expression_operation
    {
        resource_handler<_Z3_func_decl> base_;

        explicit expression_operation(_Z3_func_decl*) noexcept;

    public:

        explicit expression_operation(_Z3_ast*) noexcept;

        ~expression_operation() = default;

        expression_operation(expression_operation const&) = delete;
        expression_operation& operator=(expression_operation const&) = delete;

        expression_operation(expression_operation&&) = delete;
        expression_operation& operator=(expression_operation&&) = delete;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator _Z3_func_decl*() const noexcept;

        template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
            requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_WIDTHS) > 0) && ((DOMAIN_WIDTHS > 0) &&...)
        [[nodiscard]] static expression_operation create(std::string const&) noexcept;
    };
}
