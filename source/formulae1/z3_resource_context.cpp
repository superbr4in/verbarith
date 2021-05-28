#include "z3_resource_context.hpp"

namespace fml
{
    z3_resource_context::z3_resource_context() noexcept :
        base_(Z3_mk_context_rc(Z3_mk_config()))
    { }

    z3_resource_context::~z3_resource_context() noexcept
    {
        Z3_del_context(base_);
    }

    z3_resource_context::operator _Z3_context*() const noexcept
    {
        return base_;
    }

    z3_resource_context const& z3_resource_context::instance() noexcept
    {
        static z3_resource_context const context;

        return context;
    }
}
