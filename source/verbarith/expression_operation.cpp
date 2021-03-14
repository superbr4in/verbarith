#include <verbarith/expression_operation.hpp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    expression_operation::expression_operation(_Z3_func_decl* const base) noexcept :
        base_(base)
    { }

    expression_operation::expression_operation(_Z3_ast* const base) noexcept :
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        expression_operation(resource_context::apply(Z3_get_app_decl, reinterpret_cast<_Z3_app*>(base)))
    { }

    expression_operation::operator _Z3_func_decl*() const noexcept
    {
        return base_.value();
    }
}
