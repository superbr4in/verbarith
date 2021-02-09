#pragma once

#include <functional>
#include <string>

#include <z3.h>

namespace vra
{
    template <typename F, typename... Ts>
    concept expression_operation_maker =
        requires (F&& f, _Z3_context* const context, Ts&&... ts)
        {
            { std::invoke(std::forward<F>(f), context, std::forward<Ts>(ts)...) } -> std::same_as<_Z3_func_decl*>;
        };

    class expression_operation
    {
        _Z3_func_decl* base_;

        template <typename... Arguments, expression_operation_maker<Arguments...> Maker>
        explicit expression_operation(Maker&&, Arguments&&...);

    public:

        explicit expression_operation(_Z3_ast* base) noexcept;
        ~expression_operation() noexcept;

        expression_operation(expression_operation const&) = delete;
        expression_operation& operator=(expression_operation const&) = delete;

        expression_operation(expression_operation&&) = delete;
        expression_operation& operator=(expression_operation&&) = delete;

        template <std::size_t RANGE_WIDTH, std::size_t... DOMAIN_WIDTHS>
            requires (RANGE_WIDTH > 0) && (sizeof...(DOMAIN_WIDTHS) > 0) && ((DOMAIN_WIDTHS > 0) &&...)
        static expression_operation create(std::string const&) noexcept;

        operator _Z3_func_decl*() const; // NOLINT [hicpp-explicit-conversions]
    };
}
