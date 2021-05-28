#pragma once

#include <functional>

#include "z3_resource_context.hpp"

namespace fml
{
    template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
    std::invoke_result_t<Applicator, _Z3_context*, Arguments...> z3_resource_context::apply(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        return std::invoke(std::forward<Applicator>(applicator), instance().base_, std::forward<Arguments>(arguments)...);
    }
}
