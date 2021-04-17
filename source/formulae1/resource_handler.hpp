#pragma once

#include <memory>

#include <z3.h>

namespace fml
{
    template <typename Resource, typename ResourceBase, void INC(_Z3_context*, ResourceBase*), void DEC(_Z3_context*, ResourceBase*)>
    class resource_handler
    {
        struct resource_deleter
        {
            void operator()(Resource*) const noexcept;
        };
        class resource_pointer : std::unique_ptr<Resource, resource_deleter>
        {
        public:

            explicit resource_pointer(Resource*) noexcept;
            void reset(Resource*) noexcept;

            using std::unique_ptr<Resource, resource_deleter>::get;
        };

        resource_pointer resource_;

    public:

        explicit resource_handler(Resource*) noexcept;

        ~resource_handler() noexcept = default;

        resource_handler(resource_handler const&) noexcept;
        resource_handler& operator=(resource_handler const&) noexcept;

        resource_handler(resource_handler&&) noexcept = default;
        resource_handler& operator=(resource_handler&&) noexcept = default;

        [[nodiscard]] Resource* value() const noexcept;
        void value(Resource*) noexcept;
    };
}
