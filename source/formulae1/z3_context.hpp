#pragma once

#include <z3.h>

namespace fml
{
    class z3_context
    {
        template <typename, typename ValueBase, void(_Z3_context*, ValueBase*), void(_Z3_context*, ValueBase*)>
        friend class z3_resource;

        _Z3_context* base_;

        z3_context() noexcept;

    public:
        ~z3_context() noexcept;

        z3_context(z3_context const&) = delete;
        z3_context& operator=(z3_context const&) = delete;

        z3_context(z3_context&&) = delete;
        z3_context& operator=(z3_context&&) = delete;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator _Z3_context*() const noexcept;

    private:
        [[nodiscard]] static z3_context const& instance() noexcept;
    };
}
