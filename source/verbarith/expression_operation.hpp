#pragma once

#include <z3.h>

#include <verbarith/resource_handler.hpp>

namespace vra
{
    class expression_operation : resource_handler<_Z3_func_decl>
    {
        using resource_handler::resource_handler;

    public:

        explicit expression_operation(_Z3_ast* base) noexcept;

        operator _Z3_func_decl*() const noexcept; // NOLINT [hicpp-explicit-conversions]

        template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
            requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_WIDTHS) > 0) && ((DOMAIN_WIDTHS > 0) &&...)
        static expression_operation create(std::string const&) noexcept;
    };
}
