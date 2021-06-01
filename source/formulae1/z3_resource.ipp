#include "z3_resource.hpp"
#include "z3_resource_context.hpp"

namespace fml
{
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    void z3_resource<Value, ValueBase, INC, DEC>::deleter::deleter::operator()(Value* const value) const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        DEC(z3_resource_context::instance(), reinterpret_cast<ValueBase*>(value));
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    z3_resource<Value, ValueBase, INC, DEC>::pointer::pointer(Value* const value) noexcept :
        std::unique_ptr<Value, deleter>(value)
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        INC(z3_resource_context::instance(), reinterpret_cast<ValueBase*>(get()));
    }
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    void z3_resource<Value, ValueBase, INC, DEC>::pointer::reset(Value* const value) noexcept
    {
        std::unique_ptr<Value, deleter>::reset(value);

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        INC(z3_resource_context::instance(), reinterpret_cast<ValueBase*>(get()));
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
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
    z3_resource<Value, ValueBase, INC, DEC>::z3_resource(Applicator&& applicator, Arguments&&... arguments) noexcept :
        base_(std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), std::forward<Arguments>(arguments)...))
    { }
    template <typename Value>
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
    z3_resource<Value>::z3_resource(Applicator&& applicator, Arguments&&... arguments) noexcept :
        base_(std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), std::forward<Arguments>(arguments)...))
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
    template <typename... Arguments, invocable<_Z3_context*, Value*, Arguments...> Applicator>
    std::invoke_result_t<Applicator, _Z3_context*, Value*, Arguments...> z3_resource<Value, ValueBase, INC, DEC>::apply(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        return std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), base_.get(), std::forward<Arguments>(arguments)...);
    }
    template <typename Value>
    template <typename... Arguments, invocable<_Z3_context*, Value*, Arguments...> Applicator>
    std::invoke_result_t<Applicator, _Z3_context*, Value*, Arguments...> z3_resource<Value>::apply(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        return std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), base_, std::forward<Arguments>(arguments)...);
    }

    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
    void z3_resource<Value, ValueBase, INC, DEC>::update_2(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        base_.reset(std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), std::forward<Arguments>(arguments)...));
    }
    template <typename Value>
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Arguments...> Applicator>
    void z3_resource<Value>::update_2(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        base_ = std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), std::forward<Arguments>(arguments)...);
    }
    template <typename Value, typename ValueBase, void INC(_Z3_context*, ValueBase*), void DEC(_Z3_context*, ValueBase*)>
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Value*, Arguments...> Applicator>
    void z3_resource<Value, ValueBase, INC, DEC>::update(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        base_.reset(std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), base_.get(), std::forward<Arguments>(arguments)...));
    }
    template <typename Value>
    template <typename... Arguments, invocable_result<Value*, _Z3_context*, Value*, Arguments...> Applicator>
    void z3_resource<Value>::update(Applicator&& applicator, Arguments&&... arguments) noexcept
    {
        base_ = std::invoke(std::forward<Applicator>(applicator), z3_resource_context::instance(), base_, std::forward<Arguments>(arguments)...);
    }
}
