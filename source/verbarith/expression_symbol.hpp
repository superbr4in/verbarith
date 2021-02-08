#pragma once

#include <string>

#include <z3.h>

namespace vra
{
    class expression_symbol
    {
        _Z3_symbol* base_;

    public:

        explicit expression_symbol(std::string const& value) noexcept;
        ~expression_symbol() = default;

        expression_symbol(expression_symbol const&) = delete;
        expression_symbol& operator=(expression_symbol const&) = delete;

        expression_symbol(expression_symbol&&) = delete;
        expression_symbol& operator=(expression_symbol&&) = delete;

        operator _Z3_symbol*() const; // NOLINT [hicpp-explicit-conversions]
    };
}
