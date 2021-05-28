#pragma once

#include <memory>

#include <z3.h>

namespace fml
{
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

        template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
            requires std::same_as<std::invoke_result_t<Applicator, _Z3_context*, Arguments...>, Value*>
        explicit z3_resource(Applicator&&, Arguments&&...) noexcept;

        ~z3_resource() noexcept = default;

        z3_resource(z3_resource const&) noexcept;
        z3_resource& operator=(z3_resource const&) noexcept;

        z3_resource(z3_resource&&) noexcept = default;
        z3_resource& operator=(z3_resource&&) noexcept = default;

        // NOLINTNEXTLINE [hicpp-explicit-conversions]
        [[nodiscard]] operator Value*() const noexcept;

        template <typename... Arguments, std::invocable<_Z3_context*, Value*, Arguments...> Applicator>
        [[nodiscard]] std::invoke_result_t<Applicator, _Z3_context*, Value*, Arguments...> apply(Applicator&&, Arguments&&...) noexcept;

        template <typename... Arguments, std::invocable<_Z3_context*, Arguments...> Applicator>
            requires std::same_as<std::invoke_result_t<Applicator, _Z3_context*, Arguments...>, Value*>
        void update_2(Applicator&&, Arguments&&...) noexcept;
        template <typename... Arguments, std::invocable<_Z3_context*, Value*, Arguments...> Applicator>
            requires std::same_as<std::invoke_result_t<Applicator, _Z3_context*, Value*, Arguments...>, Value*>
        void update(Applicator&&, Arguments&&...) noexcept;
    };
}
