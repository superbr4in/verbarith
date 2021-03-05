#include <verbarith/expression_base.hpp>
#include <verbarith/resource_handler.ipp>

namespace vra
{
    expression_base::~expression_base() noexcept = default;

    bool expression_base::conclusive() const noexcept
    {
        return apply(Z3_is_numeral_ast, base());
    }
    bool expression_base::operator==(expression_base const& other) const noexcept
    {
        return apply(Z3_is_eq_ast, base(), other.base());
    }

    std::ostream& operator<<(std::ostream& stream, expression_base const& expression) noexcept
    {
        stream << expression_base::apply(Z3_ast_to_string, expression.base());

        return stream;
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    std::size_t hash<vra::expression_base>::operator()(vra::expression_base const& expression) const noexcept
    {
        return vra::expression_base::apply(Z3_get_ast_hash, expression.base());
    }
}
