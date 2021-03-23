#pragma once

#include <verbarith/expression_operation.hpp>
#include <verbarith/expression_sort.ipp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    template <std::size_t WIDTH, std::size_t... WIDTHS, std::same_as<expression_sort>... SORTS>
    static std::array<_Z3_sort*, sizeof...(WIDTHS) + sizeof...(SORTS) + 1> make_sorts(SORTS const&... sorts) noexcept
    {
        if constexpr (sizeof...(WIDTHS) == 0)
        {
            return { sorts..., expression_sort::instance<WIDTH>() };
        }
        else
        {
            return make_sorts<WIDTHS...>(sorts..., expression_sort::instance<WIDTH>());
        }
    }
    template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
        requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_WIDTHS) > 0) && ((DOMAIN_WIDTHS > 0) &&...)
    expression_operation expression_operation::create(_Z3_symbol* const symbol) noexcept
    {
        auto const domain_sorts = make_sorts<DOMAIN_WIDTHS...>();

        return expression_operation(
            resource_context::apply(
                Z3_mk_func_decl,
                symbol,
                domain_sorts.size(),
                domain_sorts.data(),
                expression_sort::instance<RANGE_WIDTH>()));
    }
    template <std::size_t RANGE_WIDTH, std::same_as<expression_sort>... DOMAIN_SORTS>
        requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_SORTS) > 0)
    expression_operation expression_operation::create(_Z3_symbol* const symbol, DOMAIN_SORTS const&... _domain_sorts) noexcept
    {
        std::array<_Z3_sort*, sizeof...(DOMAIN_SORTS)> const domain_sorts { _domain_sorts... };

        return expression_operation(
            resource_context::apply(
                Z3_mk_func_decl,
                symbol,
                domain_sorts.size(),
                domain_sorts.data(),
                expression_sort::instance<RANGE_WIDTH>()));
    }
}
