#include "Tokens/Token_Immediate_Decimal.hpp"

#include <limits>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Decimal::Decimal(std::string &immediate, std::string &t_file, size_t line_number, size_t column)
    : Immediate(immediate, immediate, t_file, line_number, column)
{
    if (immediate.length() > 7) {
        is_valid = false;
    }
    else {
        if (immediate.at(0) == '#') {
            immediate.erase(0, 1);
        }

        try {
            size_t check = 0;
            const int v = std::stoi(immediate, &check, 10);
            if (check != immediate.length() || v > std::numeric_limits<int16_t>::max() ||
                v < std::numeric_limits<int16_t>::min()) {
                is_valid = false;
            }
            else {
                value = static_cast<std::int16_t>(v);
            }
        }
        catch (const std::invalid_argument &e) {
            is_valid = false;
        }
    }

    if (!is_valid) {
        Diagnostics::Diagnostic diag(
            Diagnostics::FileContext(file, at_line, at_column),
            "Invalid literal for 16 bit signed base 10 value",
            Diagnostics::INVALID_LITERAL, Diagnostics::ERROR
        );

        diag.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(file, at_line, at_column),
                    '^', "Found here", lexed_lines[file].at(at_line - 1)
                ), '~', token.length()
            )
        );

        Diagnostics::push(diag);
    }
}
