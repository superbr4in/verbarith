#pragma once

#include <z3.h>

#include <verbarith/expression_base.hpp>

namespace vra
{
    class expression_sort : expression_base<_Z3_sort>
    {
        using expression_base::expression_base;

    public:

        explicit expression_sort(_Z3_ast* base) noexcept;

        operator _Z3_sort*() const noexcept; // NOLINT [hicpp-explicit-conversions]

        template <std::size_t WIDTH>
            requires (WIDTH > 0)
        static expression_sort const& instance() noexcept;
    };
}
