#pragma once

#include <z3.h>

#include <formulae1/resource_handler.hpp>

namespace vra
{
    class expression_sort
    {
        resource_handler<_Z3_sort, _Z3_ast, Z3_inc_ref, Z3_dec_ref> base_;

        explicit expression_sort(_Z3_sort*) noexcept;

    public:

        explicit expression_sort(_Z3_ast*) noexcept;

        ~expression_sort() = default;

        expression_sort(expression_sort const&) = delete;
        expression_sort& operator=(expression_sort const&) = delete;

        expression_sort(expression_sort&&) = delete;
        expression_sort& operator=(expression_sort&&) = delete;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator _Z3_sort*() const noexcept;

        [[nodiscard]] static expression_sort const& boolean() noexcept;

        template <std::size_t WIDTH>
            requires (WIDTH > 0)
        [[nodiscard]] static expression_sort const& instance() noexcept;
    };
}
