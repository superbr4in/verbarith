#include <formulae1/expression_sort.hpp>
#include <formulae1/resource_handler.ipp>

namespace fml
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

    expression_sort const& expression_sort::boolean() noexcept
    {
        static expression_sort const sort(resource_context::apply(Z3_mk_bool_sort));

        return sort;
    }
}
