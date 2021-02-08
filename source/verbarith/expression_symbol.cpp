#include <verbarith/expression_context.hpp>
#include <verbarith/expression_symbol.hpp>

namespace vra
{
    expression_symbol::expression_symbol(std::string const& value) noexcept :
        base_(Z3_mk_string_symbol(expression_context::instance(), value.c_str()))
    { }

    expression_symbol::operator _Z3_symbol*() const
    {
        return base_;
    }
}
