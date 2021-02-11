#pragma once

#include <verbarith/expression_sort.hpp>

namespace vra
{
    template <std::size_t WIDTH>
        requires (WIDTH > 0)
    expression_sort const& expression_sort::instance() noexcept
    {
        static expression_sort const sort(Z3_mk_bv_sort, WIDTH);
        return sort;
    }
}
