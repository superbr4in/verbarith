#include <formulae1/expression_solver.hpp>

#include "z3_resource.ipp"

namespace fml
{
    expression_solver::expression_solver() noexcept :
        base_(std::make_unique<z3_solver>(Z3_mk_simple_solver))
    { }

    expression_solver::~expression_solver() noexcept = default;

    expression_solver::expression_solver(expression_solver const& other) noexcept :
        base_(std::make_unique<z3_solver>(*other.base_))
    { }
    expression_solver& expression_solver::operator=(expression_solver const& other) noexcept
    {
        if (&other != this)
            base_ = std::make_unique<z3_solver>(*other.base_);

        return *this;
    }

    expression_solver::expression_solver(expression_solver&&) noexcept = default;
    expression_solver& expression_solver::operator=(expression_solver&&) noexcept = default;

    std::optional<expression_model> expression_solver::check(expression<bool> const& value) const
    {
        auto* const value_resource = static_cast<_Z3_ast*>(*value.base_);

        switch (base_->apply(Z3_solver_check_assumptions, 1, &value_resource))
        {
        case Z3_L_FALSE:
            return std::nullopt;
        case Z3_L_TRUE:
            return expression_model(z3_model(base_->apply(Z3_solver_get_model)));

        default:
            throw std::logic_error("Invalid expression");
        }
    }
}
