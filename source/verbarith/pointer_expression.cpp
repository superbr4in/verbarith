#include <verbarith/expression_operation.ipp>
#include <verbarith/pointer_expression.hpp>
#include <verbarith/preprocessor_types.hpp>

namespace vra
{
    template <integral_expression_typename T>
    expression<T> pointer_expression<T>::dereference() const noexcept
    {
        static auto const indirection = expression_operation::create<widthof(T), widthof(std::uintptr_t)>("deref" + std::to_string(widthof(T)));

        auto* const resource = base();

        return expression<T>(resource_context::apply(Z3_mk_app, indirection, 1, &resource));
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    template <vra::expression_typename T>
    std::size_t hash<vra::pointer_expression<T>>::operator()(vra::pointer_expression<T> const& value) const noexcept
    {
        static hash<vra::expression_base> constexpr base_hasher;
        return base_hasher(value);
    }
}

#define POINTER_EXPRESSION(T) pointer_expression<TYPE(T)>

#define INSTANTIATE_POINTER_EXPRESSION(T)\
    template class           vra::POINTER_EXPRESSION(T) ;\
    template class std::hash<vra::POINTER_EXPRESSION(T)>;
LOOP_TYPES_0(INSTANTIATE_POINTER_EXPRESSION)
