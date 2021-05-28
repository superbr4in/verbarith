#pragma once

#include <formulae1/expression_model.hpp>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_solver;

extern "C"
{
    void Z3_solver_inc_ref(_Z3_context*, _Z3_solver*);
    void Z3_solver_dec_ref(_Z3_context*, _Z3_solver*);
}

namespace fml
{
    using z3_solver = z3_resource<_Z3_solver, _Z3_solver, Z3_solver_inc_ref, Z3_solver_dec_ref>;

    class expression_solver
    {
        std::unique_ptr<z3_solver> base_;

    public:

        expression_solver() noexcept;

        ~expression_solver() noexcept;

        expression_solver(expression_solver const&) noexcept;
        expression_solver& operator=(expression_solver const&) noexcept;

        expression_solver(expression_solver&&) noexcept;
        expression_solver& operator=(expression_solver&&) noexcept;

        [[nodiscard]] std::optional<expression_model> check(expression<bool> const&) const;
    };
}
