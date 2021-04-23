#pragma once

#include <formulae1/expression_operation.hpp>
#include <formulae1/expression_sort.ipp>
#include <formulae1/resource_handler.ipp>

namespace fml
{
    template <std::size_t SIZE, std::size_t... SIZES, std::same_as<expression_sort>... SORTS>
    static std::array<_Z3_sort*, sizeof...(SIZES) + sizeof...(SORTS) + 1> make_sorts(SORTS const&... sorts) noexcept
    {
        if constexpr (sizeof...(SIZES) == 0)
        {
            return { sorts..., expression_sort::instance<SIZE>() };
        }
        else
        {
            return make_sorts<SIZES...>(sorts..., expression_sort::instance<SIZE>());
        }
    }
    template <std::size_t RANGE_SIZE, std::size_t... DOMAIN_SIZES>
        requires (RANGE_SIZE > 0) && (sizeof...(DOMAIN_SIZES) > 0) && ((DOMAIN_SIZES > 0) &&...)
    expression_operation expression_operation::create(_Z3_symbol* const symbol) noexcept
    {
        auto const domain_sorts = make_sorts<DOMAIN_SIZES...>();

        return expression_operation(
            resource_context::apply(
                Z3_mk_func_decl,
                symbol,
                domain_sorts.size(),
                domain_sorts.data(),
                expression_sort::instance<RANGE_SIZE>()));
    }
    template <std::size_t RANGE_SIZE, std::same_as<expression_sort>... DOMAIN_SORTS>
        requires (RANGE_SIZE > 0) && (sizeof...(DOMAIN_SORTS) > 0)
    expression_operation expression_operation::create(_Z3_symbol* const symbol, DOMAIN_SORTS const&... _domain_sorts) noexcept
    {
        std::array<_Z3_sort*, sizeof...(DOMAIN_SORTS)> const domain_sorts { _domain_sorts... };

        return expression_operation(
            resource_context::apply(
                Z3_mk_func_decl,
                symbol,
                domain_sorts.size(),
                domain_sorts.data(),
                expression_sort::instance<RANGE_SIZE>()));
    }
}
