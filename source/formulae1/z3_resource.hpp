#pragma once

#include <concepts>
#include <functional>
#include <memory>

#include <z3.h>

namespace fml
{
    template <typename Function, typename... Arguments>
    concept invocable =
        requires (Function&& function, Arguments&&... arguments)
        {
            { std::invoke(std::forward<Function>(function), std::forward<Arguments>(arguments)...) };
        };
    template <typename Function, typename Result, typename... Arguments>
    concept invocable_result =
        requires (Function&& function, Arguments&&... arguments)
        {
            { std::invoke(std::forward<Function>(function), std::forward<Arguments>(arguments)...) } -> std::same_as<Result>;
        };

    template <typename Value, typename ValueBase, void (_Z3_context*, ValueBase*), void (_Z3_context*, ValueBase*)>
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

        template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
        explicit z3_resource(Applicator&&, Arguments&&...) noexcept;

        ~z3_resource() noexcept = default;

        z3_resource(z3_resource const&) noexcept;
        z3_resource& operator=(z3_resource const&) noexcept;

        z3_resource(z3_resource&&) noexcept = default;
        z3_resource& operator=(z3_resource&&) noexcept = default;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator Value*() const noexcept;

        template <typename... Arguments, invocable<_Z3_context*, Value*, Arguments...> Applicator>
        [[nodiscard]] std::invoke_result_t<Applicator, _Z3_context*, Value*, Arguments...> apply(Applicator&&, Arguments&&...) noexcept;

        template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
        void update_2(Applicator&&, Arguments&&...) noexcept;
        template <typename... Arguments, invocable_result<Value*, _Z3_context*, Value*, Arguments...> Applicator>
        void update(Applicator&&, Arguments&&...) noexcept;
    };
}
