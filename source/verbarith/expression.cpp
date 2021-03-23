#include <algorithm>
#include <regex>

#include <verbarith/expression.hpp>
#include <verbarith/expression_operation.ipp>
#include <verbarith/preprocessor_types.hpp>

namespace vra
{
    // NOLINTNEXTLINE [cppcoreguidelines-avoid-non-const-global-variables]
    static _Z3_symbol* const indirection_symbol = resource_context::apply(Z3_mk_string_symbol, "deref");

    template <integral_expression_typename T>
    static expression<T> const zero(static_cast<T>(0));
    template <integral_expression_typename T>
    static expression<T> const one(static_cast<T>(1));

    template <typename T>
    bool operator==(expression<T> const& expression_1, expression<T> const& expression_2) noexcept
    {
        return resource_context::apply(Z3_is_eq_ast, expression_1.base(), expression_2.base());
    }
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, expression<T> const& expression) noexcept
    {
        std::string const representation(resource_context::apply(Z3_ast_to_string, expression.base()));

        // Remove line breaks and indent
        stream << std::regex_replace(representation, std::regex("\\n\\s*"), " ");

        return stream;
    }

    expression<>::expression(_Z3_ast* const resource) noexcept :
        base_(std::make_unique<resource_handler<_Z3_ast>>(resource))
    { }

    expression<>::~expression() noexcept = default;

    template <integral_expression_typename T>
    expression<>::expression(expression<T> const& other) noexcept :
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        expression(reinterpret_cast<expression const&>(other))
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));
    }
    expression<>::expression(expression const& other) noexcept :
        base_(std::make_unique<resource_handler<_Z3_ast>>(other.base()))
    { }
    template <integral_expression_typename T>
    expression<>& expression<>::operator=(expression<T> const& other)
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (widthof(T) != other.width())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return operator=(reinterpret_cast<expression const&>(other));
    }
    expression<>& expression<>::operator=(expression const& other) noexcept
    {
        if (&other != this)
            base_ = std::make_unique<resource_handler<_Z3_ast>>(other.base());

        return *this;
    }

    template <integral_expression_typename T>
    expression<>::expression(expression<T>&& other) noexcept :
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        expression(reinterpret_cast<expression&&>(other))
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));
    }
    expression<>::expression(expression&&) noexcept = default;
    template <integral_expression_typename T>
    expression<>& expression<>::operator=(expression<T>&& other)
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (widthof(T) != other.width())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return operator=(reinterpret_cast<expression&&>(other));
    }
    expression<>& expression<>::operator=(expression&&) noexcept = default;

    template <integral_expression_typename T>
    expression<T> const& expression<>::as_expression() const
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (widthof(T) != width())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return reinterpret_cast<expression<T> const&>(*this);
    }
    template <integral_expression_typename T>
    expression<T>& expression<>::as_expression()
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (widthof(T) != width())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return reinterpret_cast<expression<T>&>(*this);
    }

    bool expression<>::conclusive() const noexcept
    {
        return resource_context::apply(Z3_is_numeral_ast, base());
    }

    template <integral_expression_typename T>
    T expression<>::evaluate() const
    {
        if (widthof(T) != width())
            throw std::logic_error("Invalid width");

        if (std::uint64_t value { }; resource_context::apply(Z3_get_numeral_uint64, base(), &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    std::unordered_set<std::string> expression<>::dependencies() const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        auto* const base_application = reinterpret_cast<_Z3_app*>(base());

        auto const argument_count = resource_context::apply(Z3_get_app_num_args, base_application);
        if (argument_count == 0 && !conclusive())
        {
            // Dependency by itself
            return { resource_context::apply(Z3_get_symbol_string, expression_operation(base()).symbol()) };
        }

        std::unordered_set<std::string> dependencies;
        for (std::size_t argument_index = 0; argument_index < argument_count; ++argument_index)
        {
            expression const child(resource_context::apply(Z3_get_app_arg, base_application, argument_index));

            // Recurse
            dependencies.merge(child.dependencies());
        }

        return dependencies;
    }
    std::unordered_set<expression<>> expression<>::dependencies_indirect() const noexcept
    {
        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        auto* const base_application = reinterpret_cast<_Z3_app*>(base());

        auto const argument_count = resource_context::apply(Z3_get_app_num_args, base_application);
        if (argument_count == 1 && expression_operation(base()).symbol() == indirection_symbol)
        {
            // Dependency by itself
            return { expression(resource_context::apply(Z3_get_app_arg, base_application, 0)) };
        }

        std::unordered_set<expression> dependencies;
        for (std::size_t argument_index = 0; argument_index < argument_count; ++argument_index)
        {
            expression const child(resource_context::apply(Z3_get_app_arg, base_application, argument_index));

            // Recurse
            dependencies.merge(child.dependencies_indirect());
        }

        return dependencies;
    }

    void expression<>::substitute(std::string const& key_symbol, expression const& value) noexcept
    {
        expression const key(
            resource_context::apply(
                Z3_mk_const,
                resource_context::apply(
                    Z3_mk_string_symbol,
                    key_symbol.c_str()),
                expression_sort(value.base())));

        auto* const key_resource = key.base();
        auto* const value_resource = value.base();

        // TODO: update()
        base(resource_context::apply(Z3_substitute, base(), 1, &key_resource, &value_resource));
        base(resource_context::apply(Z3_simplify, base()));
    }
    void expression<>::substitute_indirect(expression const& key_pointer, expression<std::byte> const& value) noexcept
    {
        auto* const key_pointer_resource = key_pointer.base();

        expression<std::byte> const key(
            resource_context::apply(
                Z3_mk_app,
                expression_operation::create<widthof(std::byte)>(indirection_symbol, expression_sort(key_pointer.base())),
                1,
                &key_pointer_resource));

        auto* const key_resource = key.base();
        auto* const value_resource = value.base();

        // TODO: update()
        base(resource_context::apply(Z3_substitute, base(), 1, &key_resource, &value_resource));
        base(resource_context::apply(Z3_simplify, base()));
    }

    _Z3_ast* expression<>::base() const noexcept
    {
        return base_->value();
    }
    void expression<>::base(_Z3_ast* const resource) noexcept
    {
        base_->value(resource);
    }

    std::size_t expression<>::width() const noexcept
    {
        return resource_context::apply(Z3_get_bv_sort_size, expression_sort(base()));
    }

    template <integral_expression_typename T>
    template <typename>
    expression<T>::expression(_Z3_ast* const base) noexcept :
        expression<>(base)
    { }

    template <integral_expression_typename T>
    expression<T>::expression(T const value) noexcept :
        expression(resource_context::apply(Z3_mk_unsigned_int64, static_cast<std::uint64_t>(value), expression_sort::instance<widthof(T)>()))
    { }
    template <integral_expression_typename T>
    expression<T>::expression(std::string const& symbol) :
        expression(resource_context::apply(Z3_mk_const, resource_context::apply(Z3_mk_string_symbol, symbol.c_str()), expression_sort::instance<widthof(T)>()))
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) == widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(other.base())
    { }
    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) > widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(resource_context::apply(Z3_mk_extract, unsigned{widthof(T) - 1}, unsigned{0}, other.base()))
    {
        update(Z3_simplify);
    }
    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) < widthof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(resource_context::apply(Z3_mk_zero_ext, unsigned{widthof(T) - widthof(U)}, other.base()))
    {
        update(Z3_simplify);
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) <= widthof(T))
    expression<T> expression<T>::join(std::array<expression<U>, widthof(T) / widthof(U)> const& parts) noexcept
    {
        if constexpr (widthof(U) == widthof(T))
        {
            return expression<T>(std::get<0>(parts));
        }
        else
        {
            auto result = concatenate<T, widthof(T) / widthof(U)>(
                [&parts]<std::size_t INDEX>()
                {
                    return std::get<INDEX>(parts);
                });
            result.update(Z3_simplify);

            return result;
        }
    }
    template <integral_expression_typename T>
    template <integral_expression_typename U, std::size_t POSITION>
        requires (widthof(T) >= widthof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const noexcept
    {
        expression<U> result(resource_context::apply(Z3_mk_extract, unsigned{(widthof(U) * (POSITION + 1)) - 1}, unsigned{widthof(U) * POSITION}, base()));
        result.update(Z3_simplify);

        return result;
    }

    template <integral_expression_typename T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; resource_context::apply(Z3_get_numeral_uint64, base(), &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (widthof(U) >= widthof(std::byte))
    expression<U> expression<T>::dereference() const noexcept
    {
        static auto const indirection = expression_operation::create<widthof(std::byte), widthof(T)>(indirection_symbol);

        if constexpr (widthof(U) == widthof(std::byte))
        {
            auto* const resource = base();

            return expression<U>(resource_context::apply(Z3_mk_app, indirection, 1, &resource));
        }
        else
        {
            auto result = concatenate<U, widthof(U) / widthof(std::byte)>(
                [this]<std::size_t INDEX>()
                {
                    auto const advanced = operator+(expression(static_cast<T>(INDEX)));
                    auto* const advanced_resource = advanced.base();

                    return expression<std::byte>(resource_context::apply(Z3_mk_app, indirection, 1, &advanced_resource));
                });
            result.update(Z3_simplify);

            return result;
        }
    }

    template <integral_expression_typename T>
    expression<bool> expression<T>::equal(expression const& other) const noexcept
    {
        auto result = derive<bool>(Z3_mk_eq, other);
        result.update(Z3_mk_ite, one<bool>, zero<bool>);
        result.update(Z3_simplify);

        return result;
    }
    template <integral_expression_typename T>
    expression<bool> expression<T>::less(expression const& other) const noexcept
    {
        auto result = derive<bool>(std::signed_integral<T> ? Z3_mk_bvslt : Z3_mk_bvult, other);
        result.update(Z3_mk_ite, one<bool>, zero<bool>);
        result.update(Z3_simplify);

        return result;
    }

    template <integral_expression_typename T>
    expression<T>& expression<T>::operator++() noexcept
    {
        return operator+=(one<T>);
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator--() noexcept
    {
        return operator-=(one<T>);
    }

    template <integral_expression_typename T>
    expression<T>& expression<T>::operator +=(expression const& other) noexcept
    {
        update(Z3_mk_bvadd, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator -=(expression const& other) noexcept
    {
        update(Z3_mk_bvsub, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator *=(expression const& other) noexcept
    {
        update(Z3_mk_bvmul, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator /=(expression const& other) noexcept
    {
        update(std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator %=(expression const& other) noexcept
    {
        update(std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator &=(expression const& other) noexcept
    {
        update(Z3_mk_bvand, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator |=(expression const& other) noexcept
    {
        update(Z3_mk_bvor, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator ^=(expression const& other) noexcept
    {
        update(Z3_mk_bvxor, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator<<=(expression const& other) noexcept
    {
        update(Z3_mk_bvshl, other);
        update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator>>=(expression const& other) noexcept
    {
        update(Z3_mk_bvlshr, other);
        update(Z3_simplify);

        return *this;
    }

    template <integral_expression_typename T>
    expression<T> expression<T>::operator-() const noexcept
    {
        return derive(Z3_mk_bvneg);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator~() const noexcept
    {
        return derive(Z3_mk_bvnot);
    }

    template <integral_expression_typename T>
    expression<T> expression<T>::operator +(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvadd, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator -(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvsub, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator *(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvmul, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator /(expression const& other) const noexcept
    {
        return derive(std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator %(expression const& other) const noexcept
    {
        return derive(std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator &(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvand, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator |(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvor, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator ^(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvxor, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator<<(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvshl, other);
    }
    template <integral_expression_typename T>
    expression<T> expression<T>::operator>>(expression const& other) const noexcept
    {
        return derive(Z3_mk_bvlshr, other);
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U, typename Applicator>
    expression<U> expression<T>::derive(Applicator&& applicator) const noexcept
    {
        expression<U> derived(resource_context::apply(std::forward<Applicator>(applicator), base()));
        derived.update(Z3_simplify);

        return derived;
    }
    template <integral_expression_typename T>
    template <integral_expression_typename U, typename Applicator>
    expression<U> expression<T>::derive(Applicator&& applicator, expression const& other) const noexcept
    {
        expression<U> derived(resource_context::apply(std::forward<Applicator>(applicator), base(), other.base()));
        derived.update(Z3_simplify);

        return derived;
    }

    template <integral_expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base()));
    }
    template <integral_expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator, expression const& other) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base(), other.base()));
    }
    template <integral_expression_typename T>
    template <typename Applicator>
    void expression<T>::update(Applicator&& applicator, expression const& other_1, expression const& other_2) noexcept
    {
        base(resource_context::apply(std::forward<Applicator>(applicator), base(), other_1.base(), other_2.base()));
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U, std::size_t COUNT, typename Generator>
        requires (COUNT > 1)
    expression<U> expression<T>::concatenate(Generator const& generator) noexcept
    {
        auto const& current = generator.template operator()<COUNT - 1>();

        if constexpr (COUNT > 2)
        {
            // Recurse
            auto const previous = concatenate<U, COUNT - 1>(generator);

            return expression<U>(resource_context::apply(Z3_mk_concat, current.base(), previous.base()));
        }
        else
        {
            // Finalize
            auto const& previous = generator.template operator()<0>();

            return expression<U>(resource_context::apply(Z3_mk_concat, current.base(), previous.base()));
        }
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    std::size_t hash<vra::expression<>>::operator()(vra::expression<> const& expression) const noexcept
    {
        return vra::resource_context::apply(Z3_get_ast_hash, expression.base());
    }

    template <vra::integral_expression_typename T>
    std::size_t hash<vra::expression<T>>::operator()(vra::expression<T> const& expression) const noexcept
    {
        static hash<vra::expression<>> constexpr base_hasher;

        return base_hasher(expression);
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

#define INSTANTIATE_EXPRESSION_SQUARE_INDEXED(T, U, index)\
    template vra::EXPRESSION(U) vra::EXPRESSION(T)::extract<TYPE(U), index>() const;
#define INSTANTIATE_EXPRESSION_SQUARE(T, U)\
    IF_NOT_EQUAL(            T, U, template                    vra::EXPRESSION(T)::expression(     EXPRESSION(U) const&));\
    IF_TYPE_WIDTH_DIVIDABLE( T, U, template vra::EXPRESSION(T) vra::EXPRESSION(T)::join(std::array<EXPRESSION(U), TYPE_WIDTH_DIVIDE(T, U)> const&));\
    IF_NOT_EQUAL(            U, 0, template vra::EXPRESSION(U) vra::EXPRESSION(T)::dereference() const);\
    LOOP_TYPE_WIDTH_DIVIDE_2(T, U, INSTANTIATE_EXPRESSION_SQUARE_INDEXED, T, U);
#define INSTANTIATE_EXPRESSION(T)\
    template bool            vra::operator==(EXPRESSION(T) const&, EXPRESSION(T) const&);\
    template std::ostream&   vra::operator<<(std::ostream       &, EXPRESSION(T) const&);\
    template class           vra::EXPRESSION(T);\
    template class std::hash<vra::EXPRESSION(T)>;\
    LOOP_TYPES_1(INSTANTIATE_EXPRESSION_SQUARE, T);
LOOP_TYPES_0(INSTANTIATE_EXPRESSION);

template bool          vra::operator==(expression<> const&, expression<> const&);
template std::ostream& vra::operator<<(std::ostream      &, expression<> const&);

#define INSTANTIATE_ANONYMOUS_EXPRESSION(T)\
    template                           vra::expression<>::expression(EXPRESSION(T) const&);\
    template vra::expression< >      & vra::expression<>::operator=( EXPRESSION(T) const&);\
    template                           vra::expression<>::expression(EXPRESSION(T)&&);\
    template vra::expression< >      & vra::expression<>::operator=( EXPRESSION(T)&&);\
    template vra::EXPRESSION(T) const& vra::expression<>::as_expression() const;\
    template vra::EXPRESSION(T)      & vra::expression<>::as_expression();\
    template            TYPE(T)        vra::expression<>::evaluate() const;
LOOP_TYPES_0(INSTANTIATE_ANONYMOUS_EXPRESSION);
