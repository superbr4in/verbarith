#pragma once

#include <verbarith/expression_base.hpp>
#include <verbarith/expression_context.hpp>

namespace vra
{
    template <resource_typename Resource>
    void expression_base<Resource>::resource_deleter::resource_deleter::operator()(Resource* const resource) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_dec_ref(context(), reinterpret_cast<_Z3_ast*>(resource));
    }

    template <resource_typename Resource>
    expression_base<Resource>::resource_pointer::resource_pointer(Resource* const resource) noexcept :
        std::unique_ptr<Resource, resource_deleter>(resource)
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_inc_ref(context(), reinterpret_cast<_Z3_ast*>(resource));
    }

    template <resource_typename Resource>
    expression_base<Resource>::expression_base(Resource* const resource) noexcept :
        resource_(resource)
    { }

    template <resource_typename Resource>
    expression_base<Resource>::expression_base(expression_base const& other) noexcept :
        resource_(other.resource())
    { }
    template <resource_typename Resource>
    expression_base<Resource>& expression_base<Resource>::operator=(expression_base const& other) noexcept
    {
        if (&other != this)
            resource_ = resource_pointer(other.resource());

        return *this;
    }

    template <resource_typename Resource>
    template <typename... Arguments, resource_maker<Resource, Arguments...> Maker>
    expression_base<Resource>::expression_base(Maker&& maker, Arguments&&... arguments) :
        expression_base(std::invoke(std::forward<Maker>(maker), context(), std::forward<Arguments>(arguments)...))
    { }
    template <resource_typename Resource>
    template <typename... Arguments, resource_maker<Resource, Resource*, Arguments...> Maker>
    void expression_base<Resource>::apply(Maker&& maker, Arguments&&... arguments)
    {
        resource_ = resource_pointer(std::invoke(std::forward<Maker>(maker), context(), resource(), std::forward<Arguments>(arguments)...));
    }

    template <resource_typename Resource>
    Resource* expression_base<Resource>::resource() const
    {
        return resource_.get();
    }

    template <resource_typename Resource>
    _Z3_context* expression_base<Resource>::context()
    {
        return expression_context::instance();
    }
}
