#include "Tokens/Token_Immediate_Octal.hpp"

#include <limits>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Octal::Octal(std::string &immediate, std::string &t_file, size_t line_number,
             size_t column)
    : Immediate(immediate, immediate, t_file, line_number, column)
{
    immediate.front() = '0';

    try {
        size_t check = 0;
        int v = std::stoi(immediate, &check, 8);
        if (check != immediate.length()
            || v > std::numeric_limits<int16_t>::max()
            || v < std::numeric_limits<int16_t>::min()) {
            is_valid = false;
        } else {
            value = static_cast<int16_t>(v);
        }
    } catch (const std::invalid_argument &e) {
        is_valid = false;
    }

    if (!is_valid) {
        // TODO: Provide helpful fix-its here if possible (e.g. think of
        // changing to base 10/16, etc.)
        Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file, line, column),
            "Invalid literal for 16 bit signed base 8 value",
            Diagnostics::INVALID_LITERAL, Diagnostics::ERROR);

        diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(file, line, column), '^',
                    "Found here", lexed_lines[file].at(line)),
                '~', token.length()));

        Diagnostics::push(diagnostic);
    }
}
