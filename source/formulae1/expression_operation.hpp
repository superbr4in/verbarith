#pragma once

#include <z3.h>

#include <formulae1/expression_sort.hpp>

namespace fml
{
    class expression_operation
    {
        resource_handler<_Z3_func_decl, _Z3_ast, Z3_inc_ref, Z3_dec_ref> base_;

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

        [[nodiscard]] _Z3_symbol* symbol() const noexcept;

        template <std::size_t RANGE_SIZE, std::size_t... DOMAIN_SIZES>
            requires (RANGE_SIZE > 0) && (sizeof...(DOMAIN_SIZES) > 0) && ((DOMAIN_SIZES > 0) &&...)
        [[nodiscard]] static expression_operation create(_Z3_symbol*) noexcept;
        template <std::size_t RANGE_SIZE, std::same_as<expression_sort>... DOMAIN_SORTS>
            requires (RANGE_SIZE > 0) && (sizeof...(DOMAIN_SORTS) > 0)
        [[nodiscard]] static expression_operation create(_Z3_symbol*, DOMAIN_SORTS const&...) noexcept;
    };
}
