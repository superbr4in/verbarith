#include <verbarith/expression_sort.hpp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    expression_sort::expression_sort(_Z3_sort* const base) noexcept :
        base_(base)
    { }

    expression_sort::expression_sort(_Z3_ast* const base) noexcept :
        expression_sort(resource_context::apply(Z3_get_sort, base))
    { }

    expression_sort::operator _Z3_sort*() const noexcept
    {
        return base_.value();
    }
}
