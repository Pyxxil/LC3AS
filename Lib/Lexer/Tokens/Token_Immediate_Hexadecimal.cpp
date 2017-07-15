#include "Tokens/Token_Immediate_Hexadecimal.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Hexadecimal::Hexadecimal(std::string &immediate,
                         std::string &immediate_uppercase,
                         std::string &t_file,
                         size_t line_number,
                         size_t column)
        : Immediate(immediate, immediate_uppercase, t_file, line_number, column)
{
        if (immediate.length() > 6 || immediate.length() == 1 ||
            (immediate.length() == 2 && std::toupper(immediate.at(1)) == 'X')) {
                is_valid = false;
        } else {

                if (std::toupper(immediate.at(0)) == 'X') {
                        immediate.insert(0, 1, '0');
                }

                try {
                        size_t check = 0;
                        int    v     = std::stoi(immediate, &check, 16);
                        if (check != immediate.length() || v > std::numeric_limits<int16_t>::max() ||
                            v < std::numeric_limits<int16_t>::min()) {
                                is_valid = false;
                        } else {
                                value = static_cast<std::int16_t>(v);
                        }
                } catch (const std::invalid_argument &e) {
                        is_valid = false;
                }
        }

        if (!is_valid) {
                Diagnostics::Diagnostic diag(
                        Diagnostics::FileContext(
                                Diagnostics::Variant<std::string>(file, Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<size_t>(at_line, Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<size_t>(at_column, Console::FOREGROUND_COLOUR::YELLOW)
                        ),
                        "Invalid literal for 16 bit signed base 16 value",
                        Diagnostics::INVALID_LITERAL,
                        Diagnostics::ERROR
                );

                diag.provide_context(
                        std::make_unique<Diagnostics::HighlightContext>(
                                Diagnostics::SelectionContext(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(file,
                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(at_line,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(at_column,
                                                                             Console::FOREGROUND_COLOUR::YELLOW)
                                        ), '^', "Found here",
                                        std::string(lexed_lines[file].at(at_line - 1))
                                ), '~', token.length()
                        )
                );

                Diagnostics::push(diag);
        }
}
