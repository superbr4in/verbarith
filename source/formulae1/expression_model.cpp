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
        _Z3_ast* application_resource { };
        switch (base_->apply(Z3_model_eval, *value.base_, false, &application_resource))
        {
        case Z3_L_FALSE:
        case Z3_L_TRUE:
            return expression<T>(z3_ast(application_resource));

        default:
            throw std::logic_error("Invalid expression");
        }
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
        std::string string(base_->apply(Z3_model_to_string));

        // Remove line breaks
        string = std::regex_replace(string, std::regex(R"(\n *$)"), "");
        string = std::regex_replace(string, std::regex(R"(\n *)"), ", ");

        return string;
    }
}

#define EXPRESSION(T) expression<TYPE(T)>

template fml::expression<>      fml::expression_model::apply(expression<>     const&) const;
template fml::expression<bool>  fml::expression_model::apply(expression<bool> const&) const;

#define INSTANTIATE_APPLY(T)\
    template fml::EXPRESSION(T) fml::expression_model::apply(EXPRESSION(T)    const&) const;
LOOP_TYPES_0(INSTANTIATE_APPLY);
