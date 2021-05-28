#include <algorithm>
#include <climits>
#include <regex>

#include <formulae1/expression.hpp>

#include "preprocessor_types.hpp"
#include "z3_resource.ipp"

namespace fml
{
    using z3_app = z3_resource<_Z3_app, _Z3_app, nullptr, nullptr>;
    using z3_apply_result = z3_resource<_Z3_apply_result, _Z3_apply_result, Z3_apply_result_inc_ref, Z3_apply_result_dec_ref>;
    using z3_ast_vector = z3_resource<_Z3_ast_vector, _Z3_ast_vector, Z3_ast_vector_inc_ref, Z3_ast_vector_dec_ref>;
    using z3_func_decl = z3_resource<_Z3_func_decl, _Z3_ast, Z3_inc_ref, Z3_dec_ref>;
    using z3_goal = z3_resource<_Z3_goal, _Z3_goal, Z3_goal_inc_ref, Z3_goal_dec_ref>;
    using z3_sort = z3_resource<_Z3_sort, _Z3_ast, Z3_inc_ref, Z3_dec_ref>;
    using z3_symbol = z3_resource<_Z3_symbol, _Z3_symbol, nullptr, nullptr>;
    using z3_tactic = z3_resource<_Z3_tactic, _Z3_tactic, Z3_tactic_inc_ref, Z3_tactic_dec_ref>;

    static z3_symbol const indirection_symbol(Z3_mk_string_symbol, "deref");

    template <integral_expression_typename T>
    static expression<T> const zero(static_cast<T>(0));
    template <integral_expression_typename T>
    static expression<T> const one(static_cast<T>(1));

    template <typename T>
    expression<T> parse_expression(std::string const& string)
    {
        // TODO: Reset error handler
        z3_ast_vector parsed_string(Z3_parse_smtlib2_string, string.c_str(), 0, nullptr, nullptr, 0, nullptr, nullptr);
        if (parsed_string.apply(Z3_ast_vector_size) != 1)
            throw std::invalid_argument("Parsing error");

        z3_ast ast(Z3_ast_vector_get, parsed_string, 0);
        if constexpr (std::same_as<T, bool>)
        {
            if (z3_sort(Z3_get_sort, ast).apply(Z3_get_sort_kind) != Z3_BOOL_SORT)
                throw std::invalid_argument("Parsing error");
        }
        else if constexpr (!std::same_as<T, void>)
        {
            if (z3_sort(Z3_get_sort, ast).apply(Z3_get_bv_sort_size) / CHAR_BIT != sizeof(T))
                throw std::invalid_argument("Parsing error");
        }

        return expression<T>(std::move(ast));
    }

    template <typename T>
    bool operator==(expression<T> const& expression_1, expression<T> const& expression_2) noexcept
    {
        return expression_1.base_->apply(Z3_is_eq_ast, *expression_2.base_);
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, expression<T> const& expression) noexcept
    {
        stream << expression.representation();

        return stream;
    }
    template <typename T>
    std::wostream& operator<<(std::wostream& stream, expression<T> const& expression) noexcept
    {
        auto const representation = expression.representation();
        stream << std::wstring(representation.begin(), representation.end());

        return stream;
    }

    expression<>::expression(z3_ast base) noexcept :
        base_(std::make_unique<z3_ast>(std::move(base)))
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
        base_(std::make_unique<z3_ast>(*other.base_))
    { }
    template <integral_expression_typename T>
    expression<>& expression<>::operator=(expression<T> const& other) noexcept
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return operator=(reinterpret_cast<expression const&>(other));
    }
    expression<>& expression<>::operator=(expression const& other) noexcept
    {
        if (&other != this)
            base_ = std::make_unique<z3_ast>(*other.base_);

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
    expression<>& expression<>::operator=(expression<T>&& other) noexcept
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return operator=(reinterpret_cast<expression&&>(other));
    }
    expression<>& expression<>::operator=(expression&&) noexcept = default;

    template <integral_expression_typename T>
    expression<T> const& expression<>::as_expression() const
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (sizeof(T) != size())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return reinterpret_cast<expression<T> const&>(*this);
    }
    template <integral_expression_typename T>
    expression<T>& expression<>::as_expression()
    {
        static_assert(sizeof(expression<T>) == sizeof(expression));

        if (sizeof(T) != size())
            throw std::logic_error("Invalid width");

        // NOLINTNEXTLINE [cppcoreguidelines-pro-type-reinterpret-cast]
        return reinterpret_cast<expression<T>&>(*this);
    }

    bool expression<>::conclusive() const noexcept
    {
        return base_->apply(Z3_is_numeral_ast);
    }

    template <integral_expression_typename T>
    T expression<>::evaluate() const
    {
        if (sizeof(T) != size())
            throw std::logic_error("Invalid width");

        if (std::uint64_t value { }; base_->apply(Z3_get_numeral_uint64, &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    std::unordered_set<std::string> expression<>::dependencies() const noexcept
    {
        z3_app base_application(Z3_to_app, *base_);

        auto const argument_count = base_application.apply(Z3_get_app_num_args);
        if (argument_count == 0 && !conclusive())
        {
            // Dependency by itself
            return { z3_symbol(Z3_get_decl_name, z3_func_decl(Z3_get_app_decl, base_application)).apply(Z3_get_symbol_string) };
        }

        std::unordered_set<std::string> dependencies;
        for (std::size_t argument_index = 0; argument_index < argument_count; ++argument_index)
        {
            expression const child(z3_ast(Z3_get_app_arg, base_application, argument_index));

            // Recurse
            dependencies.merge(child.dependencies());
        }

        return dependencies;
    }
    std::unordered_set<expression<>> expression<>::dependencies_indirect() const noexcept
    {
        z3_app base_application(Z3_to_app, *base_);

        auto const argument_count = base_application.apply(Z3_get_app_num_args);
        if (argument_count == 1 && z3_func_decl(Z3_get_app_decl, base_application).apply(Z3_get_decl_name) == indirection_symbol)
        {
            // Dependency by itself
            return { expression(z3_ast(Z3_get_app_arg, base_application, 0)) };
        }

        std::unordered_set<expression> dependencies;
        for (std::size_t argument_index = 0; argument_index < argument_count; ++argument_index)
        {
            expression const child(z3_ast(Z3_get_app_arg, base_application, argument_index));

            // Recurse
            dependencies.merge(child.dependencies_indirect());
        }

        return dependencies;
    }

    void expression<>::substitute(std::string const& key_symbol, expression const& value) noexcept
    {
        expression const key(
            z3_ast(
                Z3_mk_const,
                z3_symbol(
                    Z3_mk_string_symbol,
                    key_symbol.c_str()),
                z3_sort(Z3_get_sort, *value.base_)));

        auto* const key_resource = static_cast<_Z3_ast*>(*key.base_);
        auto* const value_resource = static_cast<_Z3_ast*>(*value.base_);

        base_->update(Z3_substitute, 1, &key_resource, &value_resource);
        base_->update(Z3_simplify);
    }
    void expression<>::substitute_indirect(expression const& key_pointer, expression<std::byte> const& value) noexcept
    {
        z3_sort const key_pointer_sort(Z3_get_sort, *key_pointer.base_);

        auto* const key_pointer_resource = static_cast<_Z3_ast*>(*key_pointer.base_);
        auto* const key_pointer_sort_resource = static_cast<_Z3_sort*>(key_pointer_sort);

        expression<std::byte> const key(
            z3_ast(
                Z3_mk_app,
                z3_func_decl(
                    Z3_mk_func_decl,
                    indirection_symbol,
                    1,
                    &key_pointer_sort_resource,
                    z3_sort(
                        Z3_mk_bv_sort,
                        CHAR_BIT)),
                1,
                &key_pointer_resource));

        auto* const key_resource = static_cast<_Z3_ast*>(*key.base_);
        auto* const value_resource = static_cast<_Z3_ast*>(*value.base_);

        base_->update(Z3_substitute, 1, &key_resource, &value_resource);
        base_->update(Z3_simplify);
    }

    std::size_t expression<>::size() const noexcept
    {
        return z3_sort(Z3_get_sort, *base_).apply(Z3_get_bv_sort_size) / CHAR_BIT;
    }

    std::string expression<>::representation() const noexcept
    {
        std::string string(base_->apply(Z3_ast_to_string));

        // Remove line breaks
        string = std::regex_replace(string, std::regex(R"(\n *)"), " ");

        return string;
    }

    template <std::same_as<bool> T>
    expression<T> operator!(expression<T> value) noexcept
    {
        value.base_->update(Z3_mk_not);
        value.base_->update(Z3_simplify);

        return value;
    }

    template <std::same_as<bool> T>
    expression<T> operator&(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 &= value_2;
    }
    template <std::same_as<bool> T>
    expression<T> operator&(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 &= value_2;
    }
    template <std::same_as<bool> T>
    expression<T> operator|(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 |= value_2;
    }
    template <std::same_as<bool> T>
    expression<T> operator|(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 |= value_2;
    }
    template <std::same_as<bool> T>
    expression<T> operator^(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 ^= value_2;
    }
    template <std::same_as<bool> T>
    expression<T> operator^(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 ^= value_2;
    }

    expression<bool>::expression(z3_ast base) noexcept :
        expression<>(std::move(base))
    { }

    expression<bool>::expression(bool const value) noexcept :
        expression(z3_ast(value ? Z3_mk_true : Z3_mk_false))
    { }

    expression<bool> expression<bool>::symbol(std::string const& symbol)
    {
        // TODO: Remove this and (possibly) more duplicated code
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");

        return expression(
            z3_ast(
                Z3_mk_const,
                z3_symbol(
                    Z3_mk_string_symbol,
                    symbol.c_str()),
                z3_sort(Z3_mk_bool_sort)));
    }

    template <integral_expression_typename T>
    expression<bool>::expression(expression<T> const& other) :
        expression(!other.equals(zero<T>))
    {
        base_->update(Z3_simplify);
    }

    bool expression<bool>::evaluate() const
    {
        switch (base_->apply(Z3_get_bool_value))
        {
        case Z3_L_FALSE:
            return false;
        case Z3_L_TRUE:
            return true;

        default:
            throw std::logic_error("Inconclusive evaluation");
        }
    }

    void expression<bool>::reduce()
    {
        z3_goal reduction_goal(Z3_mk_goal, false, false, false);
        reduction_goal.apply(Z3_goal_assert, *base_);
        reduction_goal.update_2(
            Z3_apply_result_get_subgoal,
            z3_apply_result(Z3_tactic_apply, z3_tactic(Z3_mk_tactic, "ctx-simplify"), reduction_goal),
            0);

        base_->update_2(Z3_mk_true);
        auto const reduction_goal_size = reduction_goal.apply(Z3_goal_size);
        for (auto index = 0U; index < reduction_goal_size; ++index)
        {
            z3_ast formula(Z3_goal_formula, reduction_goal, index);

            std::array<_Z3_ast*, 2> const arguments { *base_, formula };
            base_->update_2(Z3_mk_and, arguments.size(), arguments.data());
        }
        base_->update(Z3_simplify);
    }

    expression<bool> expression<bool>::equals(expression const& other) const noexcept
    {
        z3_ast derived(Z3_mk_eq, *base_, *other.base_);
        derived.update(Z3_simplify);

        return expression(std::move(derived));
    }
    expression<bool> expression<bool>::implies(expression const& other) const noexcept
    {
        z3_ast derived(Z3_mk_implies, *base_, *other.base_);
        derived.update(Z3_simplify);

        return expression(std::move(derived));
    }

    expression<bool>& expression<bool>::operator&=(expression const& other) noexcept
    {
        std::array<_Z3_ast*, 2> const arguments { *base_, *other.base_ };
        base_->update_2(Z3_mk_and, arguments.size(), arguments.data());
        base_->update(Z3_simplify);

        return *this;
    }
    expression<bool>& expression<bool>::operator|=(expression const& other) noexcept
    {
        std::array<_Z3_ast*, 2> const arguments { *base_, *other.base_ };
        base_->update_2(Z3_mk_or, arguments.size(), arguments.data());
        base_->update(Z3_simplify);

        return *this;
    }
    expression<bool>& expression<bool>::operator^=(expression const& other) noexcept
    {
        base_->update(Z3_mk_xor, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }

    template <integral_expression_typename T>
    expression<T> operator-(expression<T> value) noexcept
    {
        value.base_->update(Z3_mk_bvneg);
        value.base_->update(Z3_simplify);

        return value;
    }
    template <integral_expression_typename T>
    expression<T> operator~(expression<T> value) noexcept
    {
        value.base_->update(Z3_mk_bvnot);
        value.base_->update(Z3_simplify);

        return value;
    }

    template <integral_expression_typename T>
    expression<T> operator+(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 += value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator+(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 += value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator-(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 -= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator-(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 -= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator*(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 *= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator*(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 *= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator/(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 /= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator/(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 /= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator%(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 %= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator%(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 %= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator&(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 &= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator&(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 &= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator|(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 |= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator|(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 |= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator^(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 ^= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator^(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 ^= value_2;
    }

    template <integral_expression_typename T>
    expression<T> operator<<(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 <<= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator<<(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 <<= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator>>(expression<T> value_1, expression<T> const& value_2) noexcept
    {
        return value_1 >>= value_2;
    }
    template <integral_expression_typename T>
    expression<T> operator>>(expression<T> value_1, T const value_2) noexcept
    {
        return value_1 >>= value_2;
    }

    template <integral_expression_typename T>
    expression<T>::expression(z3_ast base) noexcept :
        expression<>(std::move(base))
    { }

    template <integral_expression_typename T>
    expression<T>::expression(T const value) noexcept :
        expression(z3_ast(Z3_mk_unsigned_int64, static_cast<std::uint64_t>(value), z3_sort(Z3_mk_bv_sort, sizeof(T) * CHAR_BIT)))
    { }

    template <integral_expression_typename T>
    expression<T> expression<T>::symbol(std::string const& symbol)
    {
        if (symbol.empty() || std::isdigit(symbol.front()) != 0 || std::ranges::any_of(symbol, [](char const c) { return c == ' ' || std::isprint(c) == 0; }))
            throw std::invalid_argument("Invalid symbol");

        return expression(
            z3_ast(
                Z3_mk_const,
                z3_symbol(
                    Z3_mk_string_symbol,
                    symbol.c_str()),
                z3_sort(Z3_mk_bv_sort, sizeof(T) * CHAR_BIT)));
    }

    template <integral_expression_typename T>
    expression<T>::expression(expression<bool> const& other) noexcept :
        expression(*other.base_)
    {
        base_->update(Z3_mk_ite, *one<T>.base_, *zero<T>.base_);
        base_->update(Z3_simplify);
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (sizeof(U) == sizeof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(*other.base_)
    { }
    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (sizeof(U) > sizeof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(*other.base_)
    {
        base_->update_2(Z3_mk_extract, unsigned{sizeof(T) * CHAR_BIT - 1}, unsigned{0}, *base_);
        base_->update(Z3_simplify);
    }
    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (sizeof(U) < sizeof(T))
    expression<T>::expression(expression<U> const& other) noexcept :
        expression(*other.base_)
    {
        base_->update_2(Z3_mk_zero_ext, unsigned{(sizeof(T) - sizeof(U)) * CHAR_BIT}, *base_);
        base_->update(Z3_simplify);
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
        requires (sizeof(U) <= sizeof(T))
    expression<T> expression<T>::join(std::array<expression<U>, sizeof(T) / sizeof(U)> const& parts) noexcept
    {
        if constexpr (sizeof(U) == sizeof(T))
        {
            return expression(std::get<0>(parts));
        }
        else
        {
            auto result = concatenate<T, sizeof(T) / sizeof(U)>(
                [&parts]<std::size_t INDEX>()
                {
                    return std::get<INDEX>(parts);
                });
            result.base_->update(Z3_simplify);

            return result;
        }
    }
    template <integral_expression_typename T>
    template <integral_expression_typename U, std::size_t POSITION>
        requires (sizeof(T) >= sizeof(U) * (POSITION + 1))
    expression<U> expression<T>::extract() const noexcept
    {
        z3_ast derived(Z3_mk_extract, unsigned{(sizeof(U) * CHAR_BIT * (POSITION + 1)) - 1}, unsigned{sizeof(U) * CHAR_BIT * POSITION}, *base_);
        derived.update(Z3_simplify);

        return expression<U>(std::move(derived));
    }

    template <integral_expression_typename T>
    T expression<T>::evaluate() const
    {
        if (std::uint64_t value { }; base_->apply(Z3_get_numeral_uint64, &value))
            return static_cast<T>(value);

        throw std::logic_error("Inconclusive evaluation");
    }

    template <integral_expression_typename T>
    template <integral_expression_typename U>
    expression<U> expression<T>::dereference() const noexcept
    {
        static z3_sort const indirection_sort(Z3_mk_bv_sort, sizeof(T) * CHAR_BIT);
        // NOLINTNEXTLINE [cppcoreguidelines-avoid-non-const-global-variables]
        static auto* const indirection_sort_resource = static_cast<_Z3_sort*>(indirection_sort);
        static z3_func_decl const indirection(Z3_mk_func_decl, indirection_symbol, 1, &indirection_sort_resource, z3_sort(Z3_mk_bv_sort, CHAR_BIT));

        if constexpr (sizeof(U) == 1)
        {
            auto* const resource = static_cast<_Z3_ast*>(*base_);

            return expression<U>(z3_ast(Z3_mk_app, indirection, 1, &resource));
        }
        else
        {
            auto result = concatenate<U, sizeof(U)>(
                [this]<std::size_t INDEX>()
                {
                    auto const advanced = *this + static_cast<T>(INDEX);
                    auto* const advanced_resource = static_cast<_Z3_ast*>(*advanced.base_);

                    return expression<std::byte>(z3_ast(Z3_mk_app, indirection, 1, &advanced_resource));
                });
            result.base_->update(Z3_simplify);

            return result;
        }
    }

    template <integral_expression_typename T>
    expression<bool> expression<T>::equals(expression const& other) const noexcept
    {
        z3_ast derived(Z3_mk_eq, *base_, *other.base_);
        derived.update(Z3_simplify);

        return expression<bool>(std::move(derived));
    }
    template <integral_expression_typename T>
    expression<bool> expression<T>::less_than(expression const& other) const noexcept
    {
        z3_ast derived(std::signed_integral<T> ? Z3_mk_bvslt : Z3_mk_bvult, *base_, *other.base_);
        derived.update(Z3_simplify);

        return expression<bool>(std::move(derived));
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
        base_->update(Z3_mk_bvadd, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator -=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvsub, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator *=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvmul, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator /=(expression const& other) noexcept
    {
        base_->update(std::signed_integral<T> ? Z3_mk_bvsdiv : Z3_mk_bvudiv, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator %=(expression const& other) noexcept
    {
        base_->update(std::signed_integral<T> ? Z3_mk_bvsrem : Z3_mk_bvurem, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator &=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvand, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator |=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvor, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator ^=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvxor, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator<<=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvshl, *other.base_);
        base_->update(Z3_simplify);

        return *this;
    }
    template <integral_expression_typename T>
    expression<T>& expression<T>::operator>>=(expression const& other) noexcept
    {
        base_->update(Z3_mk_bvlshr, *other.base_);
        base_->update(Z3_simplify);

        return *this;
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

            return expression<U>(z3_ast(Z3_mk_concat, *current.base_, *previous.base_));
        }
        else
        {
            // Finalize
            auto const& previous = generator.template operator()<0>();

            return expression<U>(z3_ast(Z3_mk_concat, *current.base_, *previous.base_));
        }
    }
}

namespace std // NOLINT [cert-dcl58-cpp]
{
    size_t hash<fml::expression<>>::operator()(fml::expression<> const& expression) const noexcept
    {
        return expression.base_->apply(Z3_get_ast_hash);
    }

    size_t hash<fml::expression<bool>>::operator()(fml::expression<bool> const& expression) const noexcept
    {
        static hash<fml::expression<>> constexpr base_hasher;

        return base_hasher(expression);
    }

    template <fml::integral_expression_typename T>
    size_t hash<fml::expression<T>>::operator()(fml::expression<T> const& expression) const noexcept
    {
        static hash<fml::expression<>> constexpr base_hasher;

        return base_hasher(expression);
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

template fml::expression<> fml::parse_expression(std::string const&);
template bool              fml::operator==(expression<> const&, expression<> const&);
template std::ostream&     fml::operator<<(std::ostream      &, expression<> const&);
template std::wostream&    fml::operator<<(std::wostream     &, expression<> const&);

#define INSTANTIATE_ANONYMOUS_EXPRESSION(T)\
    template                           fml::expression<>::expression(EXPRESSION(T) const&);\
    template fml::expression< >      & fml::expression<>::operator=( EXPRESSION(T) const&);\
    template                           fml::expression<>::expression(EXPRESSION(T)&&);\
    template fml::expression< >      & fml::expression<>::operator=( EXPRESSION(T)&&);\
    template fml::EXPRESSION(T) const& fml::expression<>::as_expression() const;\
    template fml::EXPRESSION(T)      & fml::expression<>::as_expression();\
    template            TYPE(T)        fml::expression<>::evaluate() const;
LOOP_TYPES_0(INSTANTIATE_ANONYMOUS_EXPRESSION);

template fml::expression<bool> fml::parse_expression(std::string const&);
template bool                  fml::operator==(expression<bool> const&, expression<bool> const&);
template std:: ostream&        fml::operator<<(std:: ostream         &, expression<bool> const&);
template std::wostream&        fml::operator<<(std::wostream         &, expression<bool> const&);
template fml::expression<bool> fml::operator! (expression<bool>);
template fml::expression<bool> fml::operator& (expression<bool>       , expression<bool> const&);
template fml::expression<bool> fml::operator& (expression<bool>       ,            bool);
template fml::expression<bool> fml::operator| (expression<bool>       , expression<bool> const&);
template fml::expression<bool> fml::operator| (expression<bool>       ,            bool);
template fml::expression<bool> fml::operator^ (expression<bool>       , expression<bool> const&);
template fml::expression<bool> fml::operator^ (expression<bool>       ,            bool);

#define INSTANTIATE_BOOLEAN_EXPRESSION(T)\
    template fml::expression<bool>::expression(EXPRESSION(T) const&);
LOOP_TYPES_0(INSTANTIATE_BOOLEAN_EXPRESSION);

#define INSTANTIATE_EXPRESSION_SQUARE_INDEXED(T, U, index)\
    template fml::EXPRESSION(U) fml::EXPRESSION(T)::extract<TYPE(U), index>() const;
#define INSTANTIATE_EXPRESSION_SQUARE(T, U)\
    IF_NOT_EQUAL(           T, U, template                    fml::EXPRESSION(T)::expression(     EXPRESSION(U) const&));\
    IF_TYPE_SIZE_DIVIDABLE( T, U, template fml::EXPRESSION(T) fml::EXPRESSION(T)::join(std::array<EXPRESSION(U), TYPE_SIZE_DIVIDE(T, U)> const&));\
                                  template fml::EXPRESSION(U) fml::EXPRESSION(T)::dereference() const;\
    LOOP_TYPE_SIZE_DIVIDE_2(T, U, INSTANTIATE_EXPRESSION_SQUARE_INDEXED, T, U);
#define INSTANTIATE_EXPRESSION(T)\
    template fml::EXPRESSION(T) fml::parse_expression(std::string const&);\
    template bool               fml::operator==(EXPRESSION(T) const&, EXPRESSION(T) const&);\
    template std:: ostream&     fml::operator<<(std:: ostream      &, EXPRESSION(T) const&);\
    template std::wostream&     fml::operator<<(std::wostream      &, EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator- (EXPRESSION(T));\
    template fml::EXPRESSION(T) fml::operator~ (EXPRESSION(T));\
    template fml::EXPRESSION(T) fml::operator+ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator+ (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator- (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator- (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator* (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator* (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator/ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator/ (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator% (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator% (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator& (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator& (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator| (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator| (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator^ (EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator^ (EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator<<(EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator<<(EXPRESSION(T)       ,       TYPE(T));\
    template fml::EXPRESSION(T) fml::operator>>(EXPRESSION(T)       , EXPRESSION(T) const&);\
    template fml::EXPRESSION(T) fml::operator>>(EXPRESSION(T)       ,       TYPE(T));\
    template class              fml::EXPRESSION(T);\
    template class    std::hash<fml::EXPRESSION(T)>;\
    LOOP_TYPES_1(INSTANTIATE_EXPRESSION_SQUARE, T);
LOOP_TYPES_0(INSTANTIATE_EXPRESSION);
