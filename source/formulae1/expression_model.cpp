#include <regex>

#include <formulae1/expression_model.hpp>

#include "preprocessor_types.hpp"
#include "z3_resource.ipp"

namespace fml
{
    expression_model::expression_model(z3_model base) noexcept :
        base_(std::make_unique<z3_model>(std::move(base)))
    { }

    expression_model::~expression_model() noexcept = default;

    expression_model::expression_model(expression_model const& other) noexcept :
        base_(std::make_unique<z3_model>(*other.base_))
    { }
    expression_model& expression_model::operator=(expression_model const& other) noexcept
    {
        if (&other != this)
            base_ = std::make_unique<z3_model>(*other.base_);

        return *this;
    }

    expression_model::expression_model(expression_model&&) noexcept = default;
    expression_model& expression_model::operator=(expression_model&&) noexcept = default;

    template <typename T>
    expression<T> expression_model::apply(expression<T> const& value) const
    {
        if (_Z3_ast * application_resource{}; base_->apply(Z3_model_eval, *value.base_, false, &application_resource))
            return expression<T>(z3_ast(application_resource));

        throw std::logic_error("Invalid expression");
    }

    std::ostream& operator<<(std::ostream& stream, expression_model const& model) noexcept
    {
        stream << model.representation();

        return stream;
    }
    std::wostream& operator<<(std::wostream& stream, expression_model const& model) noexcept
    {
        auto const representation = model.representation();
        stream << std::wstring(representation.begin(), representation.end());

        return stream;
    }

    std::string expression_model::representation() const noexcept
    {
        static std::regex const regex_line_break_trailing(R"(\n *$)");
        static std::regex const regex_line_break(R"(\n *)");

        std::string string(base_->apply(Z3_model_to_string));

        // Remove line breaks
        string = std::regex_replace(string, regex_line_break_trailing, "");
        string = std::regex_replace(string, regex_line_break, ", ");

        return string;
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

template fml::expression<> fml::expression_model::apply(expression<> const&) const;
template fml::expression<bool> fml::expression_model::apply(expression<bool> const&) const;

#define INSTANTIATE_APPLY(T) \
    template fml::EXPRESSION(T) fml::expression_model::apply(EXPRESSION(T) const&) const;
LOOP_TYPES_0(INSTANTIATE_APPLY);
