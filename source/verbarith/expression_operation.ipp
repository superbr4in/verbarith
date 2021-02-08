#pragma once

#include <array>

#include <verbarith/expression_operation.hpp>
#include <verbarith/expression_sort.ipp>
#include <verbarith/expression_symbol.hpp>

namespace vra
{
    template <std::size_t WIDTH, std::size_t... WIDTHS, std::size_t... PROCESSED_WIDTHS>
    static std::array<_Z3_sort*, sizeof...(WIDTHS) + sizeof...(PROCESSED_WIDTHS) + 1> make_sorts(expression_sort<PROCESSED_WIDTHS> const&... processed_sorts)
    {
        if constexpr (sizeof...(WIDTHS) == 0)
        {
            return { processed_sorts..., expression_sort<WIDTH>::instance() };
        }
        else
        {
            return make_sorts<WIDTHS...>(processed_sorts..., expression_sort<WIDTH>::instance());
        }
    }

    template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
    expression_operation<RANGE_WIDTH, DOMAIN_WIDTHS...>::expression_operation(std::string const& name) noexcept :
        base_(Z3_mk_func_decl(expression_context::instance(), expression_symbol(name), sizeof...(DOMAIN_WIDTHS), make_sorts<DOMAIN_WIDTHS...>().data(),
            expression_sort<RANGE_WIDTH>::instance()))
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_inc_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }
    template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
    expression_operation<RANGE_WIDTH, DOMAIN_WIDTHS...>::~expression_operation() noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_dec_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }

    template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
    expression_operation<RANGE_WIDTH, DOMAIN_WIDTHS...>::operator _Z3_func_decl*() const
    {
        return base_;
    }
}
