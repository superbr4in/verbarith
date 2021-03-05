#pragma once

#include <memory>

struct _Z3_context; // NOLINT [cert-dcl51-cpp]

namespace vra
{
    template <typename Resource>
    concept resource_typename = std::same_as<Resource, std::remove_cvref_t<Resource>>;

    template <resource_typename Resource>
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

        private:

            void initialize() noexcept;
        };

        resource_pointer base_;

    protected:

        explicit resource_handler(Resource*) noexcept;

        virtual ~resource_handler() noexcept = 0;

        resource_handler(resource_handler const&) noexcept;
        resource_handler& operator=(resource_handler const&) noexcept;

        resource_handler(resource_handler&&) noexcept = default;
        resource_handler& operator=(resource_handler&&) noexcept = default;

        [[nodiscard]] Resource* base() const noexcept;
        void base(Resource*) noexcept;

        template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
        static std::invoke_result_t<Applicator, _Z3_context*, Arguments...> apply(Applicator&&, Arguments&&...) noexcept;
    };
}
