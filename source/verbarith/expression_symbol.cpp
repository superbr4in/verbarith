#include <verbarith/expression_symbol.hpp>

static_assert(std::is_nothrow_destructible_v<vra::expression_symbol>);
static_assert(std::is_copy_constructible_v<vra::expression_symbol>);
static_assert(std::is_copy_assignable_v<vra::expression_symbol>);
static_assert(std::is_nothrow_move_constructible_v<vra::expression_symbol>);
static_assert(std::is_nothrow_move_assignable_v<vra::expression_symbol>);
