#include <stimpak/binary.hpp>

#include <verbarith/expression_context.hpp>
#include <verbarith/expression_sort.hpp>

namespace vra
{
    template <sti::decayed_integral T>
    expression_sort<T>::expression_sort() noexcept :
        base_(Z3_mk_bv_sort(expression_context::instance(), widthof(T)))
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_inc_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }

    template <sti::decayed_integral T>
    expression_sort<T>::~expression_sort() noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        Z3_dec_ref(expression_context::instance(), reinterpret_cast<_Z3_ast*>(base_));
    }

    template <sti::decayed_integral T>
    expression_sort<T>::operator _Z3_sort*() const
    {
        return base_;
    }
}

template class vra::expression_sort<bool         >;
template class vra::expression_sort<std:: int8_t >;
template class vra::expression_sort<std::uint8_t >;
template class vra::expression_sort<std:: int16_t>;
template class vra::expression_sort<std::uint16_t>;
template class vra::expression_sort<std:: int32_t>;
template class vra::expression_sort<std::uint32_t>;
template class vra::expression_sort<std:: int64_t>;
template class vra::expression_sort<std::uint64_t>;
