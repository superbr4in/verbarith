#pragma once

#include <verbarith/expression.hpp>

namespace vra
{
    class pointer_expression : public expression<std::uintptr_t>
    {
    public:

        using expression::expression;

        template <integral_expression_typename T>
        [[nodiscard]] expression<T> dereference() const noexcept;
    };
}

namespace std
{
    template <>
    struct hash<vra::pointer_expression>
    {
        [[nodiscard]] std::size_t operator()(vra::pointer_expression const&) const noexcept;
    };
}
