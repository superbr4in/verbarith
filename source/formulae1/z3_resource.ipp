#include "z3_context.hpp"
#include "z3_resource.hpp"

namespace fml
{
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    void z3_resource<Value, ValueBase, INC, DEC>::deleter::deleter::operator()(Value* const value) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        DEC(z3_context::instance(), reinterpret_cast<ValueBase*>(value));
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>::pointer::pointer(Value* const value) noexcept :
        std::unique_ptr<Value, deleter>(value)
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        INC(z3_context::instance(), reinterpret_cast<ValueBase*>(get()));
    }
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    void z3_resource<Value, ValueBase, INC, DEC>::pointer::reset(Value* const value) noexcept
    {
        std::unique_ptr<Value, deleter>::reset(value);

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        INC(z3_context::instance(), reinterpret_cast<ValueBase*>(get()));
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>::z3_resource(Value* const base) noexcept :
        base_(base)
    { }
    template <typename Value>
    z3_resource<Value>::z3_resource(Value* const base) noexcept :
        base_(base)
    { }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments>
    z3_resource<Value, ValueBase, INC, DEC>::z3_resource(z3_invocable_output<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept :
        base_(applicator(z3_context::instance(), std::forward<Arguments>(arguments)...))
    { }
    template <typename Value>
    template <typename... Arguments>
    z3_resource<Value>::z3_resource(z3_invocable_output<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept :
        base_(applicator(z3_context::instance(), std::forward<Arguments>(arguments)...))
    { }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>::z3_resource(z3_resource const& other) noexcept :
        base_(other.base_.get())
    { }
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>& z3_resource<Value, ValueBase, INC, DEC>::operator=(z3_resource const& other) noexcept
    {
        if (&other != this)
            base_.reset(other.base_.get());

        return *this;
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>::operator Value*() const noexcept
    {
        return base_.get();
    }
    template <typename Value>
    z3_resource<Value>::operator Value*() const noexcept
    {
        return base_;
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments>
    decltype(auto) z3_resource<Value, ValueBase, INC, DEC>::apply(z3_invocable_input<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept
    {
        return applicator(z3_context::instance(), base_.get(), std::forward<Arguments>(arguments)...);
    }
    template <typename Value>
    template <typename... Arguments>
    decltype(auto) z3_resource<Value>::apply(z3_invocable_input<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept
    {
        return applicator(z3_context::instance(), base_, std::forward<Arguments>(arguments)...);
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments>
    void z3_resource<Value, ValueBase, INC, DEC>::update(z3_invocable_output<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept
    {
        base_.reset(applicator(z3_context::instance(), std::forward<Arguments>(arguments)...));
    }
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments>
    void z3_resource<Value, ValueBase, INC, DEC>::update_self(z3_invocable_input_output<Value, Arguments...> auto const& applicator, Arguments&&... arguments) noexcept
    {
        base_.reset(applicator(z3_context::instance(), base_.get(), std::forward<Arguments>(arguments)...));
    }
}
