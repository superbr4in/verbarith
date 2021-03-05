#pragma once

#include <verbarith/expression_operation.hpp>
#include <verbarith/expression_sort.ipp>

namespace vra
{
    template <std::size_t WIDTH, std::size_t... WIDTHS, std::same_as<expression_sort>... SORTS>
    static std::array<_Z3_sort*, sizeof...(WIDTHS) + sizeof...(SORTS) + 1> make_sorts(SORTS const&... sorts)
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
    expression_operation expression_operation::create(std::string const& name) noexcept
    {
        auto const domain_sorts = make_sorts<DOMAIN_WIDTHS...>();

        return expression_operation(
            apply(Z3_mk_func_decl, apply(Z3_mk_string_symbol, name.c_str()), domain_sorts.size(), domain_sorts.data(), expression_sort::instance<RANGE_WIDTH>()));
    }
}
