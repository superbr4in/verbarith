#pragma once

#include <verbarith/expression_sort.hpp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    template <std::size_t WIDTH>
        requires (WIDTH > 0)
    expression_sort const& expression_sort::instance() noexcept
    {
        static expression_sort const sort(resource_context::apply(Z3_mk_bv_sort, WIDTH));

        return sort;
    }
}
