#pragma once

#include <concepts>

#include <z3.h>

namespace fml
{
    class resource_context
    {
        _Z3_context* base_;

        resource_context() noexcept;

    public:

        ~resource_context() noexcept;

        resource_context(resource_context const&) = delete;
        resource_context& operator=(resource_context const&) = delete;

        resource_context(resource_context&&) = delete;
        resource_context& operator=(resource_context&&) = delete;

        template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
        [[nodiscard]] static std::invoke_result_t<Applicator, _Z3_context*, Arguments...> apply(Applicator&&, Arguments&&...) noexcept;

    private:

        [[nodiscard]] static resource_context const& instance() noexcept;
    };
}
