#include <iostream>
#include <sstream>

#include <formulae1/expression_solver.hpp>

std::string define_constant(char const symbol)
{
    std::ostringstream stream;
    stream << "(declare-fun " << symbol << " () Bool)";

    return stream.str();
}

int main(int const argument_count, char const* const* const arguments)
{
    if (argument_count != 2)
    {
        std::cerr << "Invalid arguments" << std::endl;

        return EXIT_FAILURE;
    }

    std::ostringstream formula_stream;
    formula_stream
        // Supported symbols
        << define_constant('A')
        << define_constant('B')
        << define_constant('C')
        << define_constant('D')
        << define_constant('E')
        << define_constant('f')
        // Formula assertion
        << "(assert " << *std::next(arguments) << ")";

    fml::expression_solver const solver;
    if (auto const model = solver.check(fml::parse_expression<bool>(formula_stream.str())); model.has_value())
    {
        // Output model
        std::cout << "Satisfiable: " << *model;
    }
    else
    {
        // No satisfying model
        std::cout << "Unsatisfiable";
    }
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
