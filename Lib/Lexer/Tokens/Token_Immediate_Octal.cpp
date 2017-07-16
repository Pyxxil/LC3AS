#include "Tokens/Token_Immediate_Octal.hpp"

#include <limits>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Octal::Octal(std::string &immediate, std::string &t_file, size_t line_number, size_t column)
        : Immediate(immediate, immediate, t_file, line_number, column)
{
        immediate.front() = '0';

        try {
                size_t check = 0;
                int    v     = std::stoi(immediate, &check, 8);
                if (check != immediate.length() || v > std::numeric_limits<int16_t>::max() ||
                    v < std::numeric_limits<int16_t>::min()) {
                        is_valid = false;
                } else {
                        value = static_cast<int16_t>(v);
                }
        } catch (const std::invalid_argument &e) {
                is_valid = false;
        }

        if (!is_valid) {
                // TODO: Provide helpful fixits here if possible (e.g. think of changing to base 10/16, etc.)
                Diagnostics::Diagnostic diag(
                        Diagnostics::FileContext(
                                Diagnostics::Variant<std::string>(file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                                Diagnostics::Variant<size_t>(at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                                Diagnostics::Variant<size_t>(at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
                        ),
                        "Invalid literal for 16 bit signed base 8 value",
                        Diagnostics::INVALID_LITERAL,
                        Diagnostics::ERROR
                );

                diag.provide_context(
                        std::make_unique<Diagnostics::HighlightContext>(
                                Diagnostics::SelectionContext(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        file,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        at_line,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        at_column,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                )
                                        ), '^', "Found here", lexed_lines[file].at(at_line - 1)
                                ), '~', token.length()
                        )
                );

                Diagnostics::push(diag);
        }
}
