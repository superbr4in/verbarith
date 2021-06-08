#pragma once

#include <concepts>
#include <functional>
#include <memory>

#include <z3.h>

namespace fml
{
    // clang-format off
    template <typename Function, typename Value, typename... Arguments>
    concept z3_invocable_input =
        requires(Function const& function, _Z3_context* const context, Value* const value, Arguments&&... arguments)
        {
            { function(context, value, std::forward<Arguments>(arguments)...) };
        };
    template <typename Function, typename Value, typename... Arguments>
    concept z3_invocable_output =
        requires(Function const& function, _Z3_context* const context, Arguments&&... arguments)
        {
            { function(context, std::forward<Arguments>(arguments)...) } -> std::same_as<Value*>;
        };
    template <typename Function, typename Value, typename... Arguments>
    concept z3_invocable_input_output =
        requires(Function const& function, _Z3_context* const context, Value* const value, Arguments&&... arguments)
        {
            { function(context, value, std::forward<Arguments>(arguments)...) } -> std::same_as<Value*>;
        };
    // clang-format on

    template <typename Value, typename ValueBase = void, void(_Z3_context*, ValueBase*) = nullptr, void(_Z3_context*, ValueBase*) = nullptr>
    class z3_resource
    {
        struct deleter
        {
            void operator()(Value*) const noexcept;
        };
        class pointer : std::unique_ptr<Value, deleter>
        {
        public:
            explicit pointer(Value*) noexcept;
            void reset(Value*) noexcept;

            using std::unique_ptr<Value, deleter>::get;
        };

        pointer base_;

    public:
        explicit z3_resource(Value*) noexcept;

        template <typename... Arguments>
        explicit z3_resource(z3_invocable_output<Value, Arguments...> auto const&, Arguments&&...) noexcept;

        ~z3_resource() noexcept = default;

        z3_resource(z3_resource const&) noexcept;
        z3_resource& operator=(z3_resource const&) noexcept;

        z3_resource(z3_resource&&) noexcept = default;
        z3_resource& operator=(z3_resource&&) noexcept = default;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator Value*() const noexcept;

        template <typename... Arguments>
        [[nodiscard]] decltype(auto) apply(z3_invocable_input<Value, Arguments...> auto const&, Arguments&&...) noexcept;

        template <typename... Arguments>
        void update(z3_invocable_output<Value, Arguments...> auto const&, Arguments&&...) noexcept;
        template <typename... Arguments>
        void update_self(z3_invocable_input_output<Value, Arguments...> auto const&, Arguments&&...) noexcept;
    };
    template <typename Value>
    class z3_resource<Value, void, nullptr, nullptr>
    {
        Value* base_;

    public:
        explicit z3_resource(Value*) noexcept;

        template <typename... Arguments>
        explicit z3_resource(z3_invocable_output<Value, Arguments...> auto const&, Arguments&&...) noexcept;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator Value*() const noexcept;

        template <typename... Arguments>
        [[nodiscard]] decltype(auto) apply(z3_invocable_input<Value, Arguments...> auto const&, Arguments&&...) noexcept;
    };
}
