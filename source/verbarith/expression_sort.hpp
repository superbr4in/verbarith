#pragma once

#include <cstddef>

#include <z3.h>

namespace vra
{
    template <std::size_t WIDTH>
    class expression_sort
    {
        _Z3_sort* base_;

        expression_sort() noexcept;

    public:

        ~expression_sort() noexcept;

        expression_sort(expression_sort const&) = delete;
        expression_sort& operator=(expression_sort const&) = delete;

        expression_sort(expression_sort&&) = delete;
        expression_sort& operator=(expression_sort&&) = delete;

        static _Z3_sort* instance() noexcept;
    };
}
