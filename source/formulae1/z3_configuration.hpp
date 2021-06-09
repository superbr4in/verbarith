#pragma once

#include <z3.h>

namespace fml
{
    class z3_configuration
    {
        _Z3_config* base_;

    public:
        z3_configuration() noexcept;

        ~z3_configuration() noexcept;

        z3_configuration(z3_configuration const&) = delete;
        z3_configuration& operator=(z3_configuration const&) = delete;

        z3_configuration(z3_configuration&&) = delete;
        z3_configuration& operator=(z3_configuration&&) = delete;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator _Z3_config*() const noexcept;
    };
}
