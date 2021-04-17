#include <formulae1/resource_context.hpp>

namespace vra
{
    resource_context::resource_context() noexcept :
        base_(Z3_mk_context_rc(Z3_mk_config()))
    { }

    resource_context::~resource_context() noexcept
    {
        Z3_del_context(base_);
    }

    resource_context const& resource_context::instance() noexcept
    {
        static resource_context const context;

        return context;
    }
}
