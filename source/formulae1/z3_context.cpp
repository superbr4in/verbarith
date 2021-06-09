#include "z3_context.hpp"
#include "z3_configuration.hpp"

namespace fml
{
    z3_context::z3_context() noexcept :
        base_(Z3_mk_context_rc(z3_configuration()))
    { }

    z3_context::~z3_context() noexcept
    {
        Z3_del_context(base_);
    }

    z3_context::operator _Z3_context*() const noexcept
    {
        return base_;
    }

    z3_context const& z3_context::instance() noexcept
    {
        static z3_context const context;

        return context;
    }
}
