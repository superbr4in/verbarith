#pragma once

#include <string>

#include <z3.h>

namespace vra
{
    template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
    class expression_operation
    {
        _Z3_func_decl* base_;

    public:

        explicit expression_operation(std::string const& name) noexcept;
        ~expression_operation() noexcept;

        expression_operation(expression_operation const&) = delete;
        expression_operation& operator=(expression_operation const&) = delete;

        expression_operation(expression_operation&&) = delete;
        expression_operation& operator=(expression_operation&&) = delete;

        operator _Z3_func_decl*() const; // NOLINT [hicpp-explicit-conversions]
    };
}
