#pragma once

#include <climits>

#include <formulae1/expression_sort.hpp>
#include <formulae1/resource_handler.ipp>

namespace fml
{
    template <std::size_t SIZE>
        requires (SIZE > 0)
    expression_sort const& expression_sort::instance() noexcept
    {
        static expression_sort const sort(resource_context::apply(Z3_mk_bv_sort, SIZE * CHAR_BIT));

        return sort;
    }
}
