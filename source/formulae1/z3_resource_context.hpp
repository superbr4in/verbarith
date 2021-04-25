#pragma once

#include <concepts>

#include <z3.h>

namespace fml
{
    class z3_resource_context
    {
        _Z3_context* base_;

        z3_resource_context() noexcept;

    public:

        ~z3_resource_context() noexcept;

        z3_resource_context(z3_resource_context const&) = delete;
        z3_resource_context& operator=(z3_resource_context const&) = delete;

        z3_resource_context(z3_resource_context&&) = delete;
        z3_resource_context& operator=(z3_resource_context&&) = delete;

        template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
        [[nodiscard]] static std::invoke_result_t<Applicator, _Z3_context*, Arguments...> apply(Applicator&&, Arguments&&...) noexcept;

    private:

        [[nodiscard]] static z3_resource_context const& instance() noexcept;
    };
}
