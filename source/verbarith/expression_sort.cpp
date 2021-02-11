#include <verbarith/expression_base.ipp>
#include <verbarith/expression_sort.hpp>

namespace vra
{
    expression_sort::expression_sort(_Z3_ast* base) noexcept :
        expression_sort(Z3_get_sort, base)
    { }

    expression_sort::operator _Z3_sort*() const noexcept
    {
        return resource();
    }
}
