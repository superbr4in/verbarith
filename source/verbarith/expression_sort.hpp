#pragma once

#include <z3.h>

#include <verbarith/resource_handler.hpp>

namespace vra
{
    class expression_sort : resource_handler<_Z3_sort>
    {
        using resource_handler::resource_handler;

    public:

        explicit expression_sort(_Z3_ast* base) noexcept;

        operator _Z3_sort*() const noexcept; // NOLINT [hicpp-explicit-conversions]

        template <std::size_t WIDTH>
            requires (WIDTH > 0)
        static expression_sort const& instance() noexcept;
    };
}
