#pragma once

#include <functional>
#include <memory>

struct _Z3_context; // NOLINT [cert-dcl51-cpp]

namespace vra
{
    template <typename Resource>
    concept resource_typename = std::same_as<Resource, std::remove_cvref_t<Resource>>;

    template <typename Function, typename Resource, typename... Arguments>
    concept resource_maker =
            resource_typename<Resource>
        &&  requires (Function&& function, _Z3_context* const context, Arguments&&... arguments)
            {
                { std::invoke(std::forward<Function>(function), context, std::forward<Arguments>(arguments)...) } -> std::same_as<Resource*>;
            };

    template <resource_typename Resource>
    class expression_base
    {
        struct resource_deleter
        {
            void operator()(Resource*) const noexcept;
        };
        class resource_pointer : std::unique_ptr<Resource, resource_deleter>
        {
        public:

            explicit resource_pointer(Resource*) noexcept;

            using std::unique_ptr<Resource, resource_deleter>::get;
        };

        resource_pointer resource_;

        explicit expression_base(Resource*) noexcept;

    protected:

        ~expression_base() noexcept = default;

        expression_base(expression_base const&) noexcept;
        expression_base& operator=(expression_base const&) noexcept;

        expression_base(expression_base&&) noexcept = default;
        expression_base& operator=(expression_base&&) noexcept = default;

        template <typename... Arguments, resource_maker<Resource, Arguments...> Maker>
        explicit expression_base(Maker&&, Arguments&&...);
        template <typename... Arguments, resource_maker<Resource, Resource*, Arguments...> Maker>
        void apply(Maker&&, Arguments&&...);

        [[nodiscard]] Resource* resource() const;

        [[nodiscard]] static _Z3_context* context();
    };
}
