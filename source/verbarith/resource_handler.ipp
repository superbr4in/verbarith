#pragma once

#include <functional>

#include <z3.h>

#include <verbarith/resource_handler.hpp>

namespace vra
{
    template <resource_typename Resource>
    void resource_handler<Resource>::resource_deleter::resource_deleter::operator()(Resource* const resource) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        apply(Z3_dec_ref, reinterpret_cast<_Z3_ast*>(resource));
    }

    template <resource_typename Resource>
    resource_handler<Resource>::resource_pointer::resource_pointer(Resource* const resource) noexcept :
        std::unique_ptr<Resource, resource_deleter>(resource)
    {
        initialize();
    }
    template <resource_typename Resource>
    void resource_handler<Resource>::resource_pointer::reset(Resource* const resource) noexcept
    {
        std::unique_ptr<Resource, resource_deleter>::reset(resource);

        initialize();
    }

    template <resource_typename Resource>
    void resource_handler<Resource>::resource_pointer::initialize() noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        apply(Z3_inc_ref, reinterpret_cast<_Z3_ast*>(get()));
    }

    template <resource_typename Resource>
    resource_handler<Resource>::resource_handler(Resource* const resource) noexcept :
        base_(resource)
    { }

    template <resource_typename Resource>
    resource_handler<Resource>::~resource_handler() noexcept = default;

    template <resource_typename Resource>
    resource_handler<Resource>::resource_handler(resource_handler const& other) noexcept :
        base_(other.base())
    { }
    template <resource_typename Resource>
    resource_handler<Resource>& resource_handler<Resource>::operator=(resource_handler const& other) noexcept
    {
        if (&other != this)
            base_.reset(other.base());

        return *this;
    }

    template <resource_typename Resource>
    Resource* resource_handler<Resource>::base() const noexcept
    {
        return base_.get();
    }
    template <resource_typename Resource>
    void resource_handler<Resource>::base(Resource* const resource) noexcept
    {
        base_.reset(resource);
    }

    static _Z3_context* context()
    {
        struct context
        {
            _Z3_context* base;

            context() noexcept :
                base(Z3_mk_context_rc(Z3_mk_config()))
            { }

            ~context() noexcept
            {
                Z3_del_context(base);
            }

            context(context const&) = delete;
            context& operator=(context const&) = delete;

            context(context&&) = delete;
            context& operator=(context&&) = delete;
        }
        static const context;

        return context.base;
    }
    template <resource_typename Resource>
    template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
    std::invoke_result_t<Applicator, _Z3_context*, Arguments...> resource_handler<Resource>::apply(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        return std::invoke(std::forward<Applicator>(applicator), context(), std::forward<Arguments>(arguments)...);
    }
}
