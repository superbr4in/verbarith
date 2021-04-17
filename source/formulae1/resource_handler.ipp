#pragma once

#include <formulae1/resource_context.ipp>
#include <formulae1/resource_handler.hpp>

namespace fml
{
    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    void resource_handler<Resource, ResourceBase, INC, DEC>::resource_deleter::resource_deleter::operator()(Resource* const resource) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        resource_context::apply(DEC, reinterpret_cast<ResourceBase*>(resource));
    }

    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    resource_handler<Resource, ResourceBase, INC, DEC>::resource_pointer::resource_pointer(Resource* const resource) noexcept :
        std::unique_ptr<Resource, resource_deleter>(resource)
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        resource_context::apply(INC, reinterpret_cast<ResourceBase*>(get()));
    }
    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    void resource_handler<Resource, ResourceBase, INC, DEC>::resource_pointer::reset(Resource* const resource) noexcept
    {
        std::unique_ptr<Resource, resource_deleter>::reset(resource);

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        resource_context::apply(INC, reinterpret_cast<ResourceBase*>(get()));
    }

    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    resource_handler<Resource, ResourceBase, INC, DEC>::resource_handler(Resource* const resource) noexcept :
        resource_(resource)
    { }

    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    resource_handler<Resource, ResourceBase, INC, DEC>::resource_handler(resource_handler const& other) noexcept :
        resource_(other.value())
    { }
    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    resource_handler<Resource, ResourceBase, INC, DEC>& resource_handler<Resource, ResourceBase, INC, DEC>::operator=(resource_handler const& other) noexcept
    {
        if (&other != this)
            resource_.reset(other.value());

        return *this;
    }

    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    Resource* resource_handler<Resource, ResourceBase, INC, DEC>::value() const noexcept
    {
        return resource_.get();
    }
    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    void resource_handler<Resource, ResourceBase, INC, DEC>::value(Resource* const resource) noexcept
    {
        resource_.reset(resource);
    }
}
