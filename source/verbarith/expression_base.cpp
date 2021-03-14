#include <verbarith/expression_base.hpp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    expression_base::expression_base(_Z3_ast* base) noexcept :
        base_(std::make_unique<resource_handler<_Z3_ast>>(base))
    { }

    expression_base::~expression_base() noexcept = default;

    expression_base::expression_base(expression_base const& other) noexcept :
        base_(std::make_unique<resource_handler<_Z3_ast>>(other.base()))
    { }
    expression_base& expression_base::operator=(expression_base const& other) noexcept
    {
        if (&other != this)
            base_ = std::make_unique<resource_handler<_Z3_ast>>(other.base());

        return *this;
    }

    bool expression_base::conclusive() const noexcept
    {
        return resource_context::apply(Z3_is_numeral_ast, base());
    }
    bool expression_base::operator==(expression_base const& other) const noexcept
    {
        return resource_context::apply(Z3_is_eq_ast, base(), other.base());
    }

    std::ostream& operator<<(std::ostream& stream, expression_base const& expression) noexcept
    {
        stream << resource_context::apply(Z3_ast_to_string, expression.base());

        return stream;
    }

    _Z3_ast* expression_base::base() const noexcept
    {
        return base_->value();
    }
    void expression_base::base(_Z3_ast* const resource) noexcept
    {
        base_->value(resource);
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    std::size_t hash<vra::expression_base>::operator()(vra::expression_base const& expression) const noexcept
    {
        return vra::resource_context::apply(Z3_get_ast_hash, expression.base());
    }
}
