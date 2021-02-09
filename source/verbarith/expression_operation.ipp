#pragma once

#include <verbarith/expression_operation.hpp>
#include <verbarith/expression_sort.ipp>
#include <verbarith/expression_symbol.hpp>

namespace vra
{
    template <typename... Arguments, expression_operation_maker<Arguments...> Maker>
    expression_operation::expression_operation(Maker&& maker, Arguments&&... arguments) :
        base_(std::invoke(std::forward<Maker>(maker), expression_context::instance(), std::forward<Arguments>(arguments)...))
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_inc_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }

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
        requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_WIDTHS) > 0) && ((DOMAIN_WIDTHS > 0) &&...)
    expression_operation expression_operation::create(std::string const& name) noexcept
    {
        auto const domain_sorts = make_sorts<DOMAIN_WIDTHS...>();
        return expression_operation(&Z3_mk_func_decl, expression_symbol(name), domain_sorts.size(), domain_sorts.data(), expression_sort<RANGE_WIDTH>::instance());
    }
}
