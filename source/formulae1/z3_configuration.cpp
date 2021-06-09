#include "z3_configuration.hpp"

namespace fml
{
    z3_configuration::z3_configuration() noexcept :
        base_(Z3_mk_config())
    { }

    z3_configuration::~z3_configuration() noexcept
    {
        Z3_del_config(base_);
    }

    z3_configuration::operator _Z3_config*() const noexcept
    {
        return base_;
    }
}
