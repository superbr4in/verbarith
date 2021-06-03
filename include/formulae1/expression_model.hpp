#pragma once

#include <formulae1/expression.hpp>

// NOLINTNEXTLINE [cert-dcl51-cpp]
struct _Z3_model;

extern "C"
{
    void Z3_model_inc_ref(_Z3_context*, _Z3_model*);
    void Z3_model_dec_ref(_Z3_context*, _Z3_model*);
}

namespace fml
{
    using z3_model = z3_resource<_Z3_model, _Z3_model, Z3_model_inc_ref, Z3_model_dec_ref>;

    class expression_model
    {
        friend class expression_solver;

        std::unique_ptr<z3_model> base_;

        explicit expression_model(z3_model) noexcept;

    public:
        ~expression_model() noexcept;

        expression_model(expression_model const&) noexcept;
        expression_model& operator=(expression_model const&) noexcept;

        expression_model(expression_model&&) noexcept;
        expression_model& operator=(expression_model&&) noexcept;

        template <typename T>
        [[nodiscard]] expression<T> apply(expression<T> const&) const;

        friend std::ostream& operator<<(std::ostream&, expression_model const&) noexcept;
        friend std::wostream& operator<<(std::wostream&, expression_model const&) noexcept;

    private:
        [[nodiscard]] std::string representation() const noexcept;
    };
}
