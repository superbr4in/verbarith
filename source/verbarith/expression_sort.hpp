#pragma once

#include <z3.h>

#include <stimpak/conceptual.hpp>

namespace vra
{
    template <sti::decayed_integral T>
    class expression_sort
    {
        _Z3_sort* base_;

    public:

        expression_sort() noexcept;

        ~expression_sort() noexcept;

        expression_sort(expression_sort const&) = delete;
        expression_sort& operator=(expression_sort const&) = delete;

        expression_sort(expression_sort&&) = delete;
        expression_sort& operator=(expression_sort&&) = delete;

        [[nodiscard]] operator _Z3_sort*() const; // NOLINT [google-explicit-constructor]
    };
}
