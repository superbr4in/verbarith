#pragma once

#include <verbarith/expression_context.hpp>
#include <verbarith/expression_sort.hpp>

namespace vra
{
    template <std::size_t WIDTH>
    expression_sort<WIDTH>::expression_sort() noexcept :
        base_(Z3_mk_bv_sort(expression_context::instance(), WIDTH))
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_inc_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }
    template <std::size_t WIDTH>
    expression_sort<WIDTH>::~expression_sort() noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_dec_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }

    template <std::size_t WIDTH>
    _Z3_sort* expression_sort<WIDTH>::instance() noexcept
    {
        static expression_sort const sort;
        return sort.base_;
    }
}
