#pragma once

#include <verbarith/resource_context.ipp>
#include <verbarith/resource_handler.hpp>

namespace vra
{
    template <typename Resource>
    void resource_handler<Resource>::resource_deleter::resource_deleter::operator()(Resource* const resource) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        resource_context::apply(Z3_dec_ref, reinterpret_cast<_Z3_ast*>(resource));
    }

    template <typename Resource>
    resource_handler<Resource>::resource_pointer::resource_pointer(Resource* const resource) noexcept :
        std::unique_ptr<Resource, resource_deleter>(resource)
    {
        initialize();
    }
    template <typename Resource>
    void resource_handler<Resource>::resource_pointer::reset(Resource* const resource) noexcept
    {
        std::unique_ptr<Resource, resource_deleter>::reset(resource);

        initialize();
    }

    template <typename Resource>
    void resource_handler<Resource>::resource_pointer::initialize() noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        resource_context::apply(Z3_inc_ref, reinterpret_cast<_Z3_ast*>(get()));
    }

    template <typename Resource>
    resource_handler<Resource>::resource_handler(Resource* const resource) noexcept :
        resource_(resource)
    { }

    template <typename Resource>
    resource_handler<Resource>::resource_handler(resource_handler const& other) noexcept :
        resource_(other.value())
    { }
    template <typename Resource>
    resource_handler<Resource>& resource_handler<Resource>::operator=(resource_handler const& other) noexcept
    {
        if (&other != this)
            resource_.reset(other.value());

        return *this;
    }

    template <typename Resource>
    Resource* resource_handler<Resource>::value() const noexcept
    {
        return resource_.get();
    }
    template <typename Resource>
    void resource_handler<Resource>::value(Resource* const resource) noexcept
    {
        resource_.reset(resource);
    }
}
