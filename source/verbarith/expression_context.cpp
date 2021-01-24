#include <type_traits>

#include <verbarith/expression_context.hpp>

namespace vra
{
    expression_context::expression_context() noexcept :
        base_(Z3_mk_context_rc(Z3_mk_config()))
    { }

    expression_context::~expression_context() noexcept
    {
        Z3_del_context(base_);
    }

    _Z3_context* expression_context::instance() noexcept
    {
        static expression_context const context;
        return context.base_;
    }
}

static_assert(std::is_nothrow_destructible_v<vra::expression_context>);
static_assert(!std::is_copy_constructible_v<vra::expression_context>);
static_assert(!std::is_copy_assignable_v<vra::expression_context>);
static_assert(!std::is_nothrow_move_constructible_v<vra::expression_context>);
static_assert(!std::is_nothrow_move_assignable_v<vra::expression_context>);
