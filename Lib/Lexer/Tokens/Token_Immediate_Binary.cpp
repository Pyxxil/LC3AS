#include "Tokens/Token_Immediate_Binary.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Binary::Binary(std::string &immediate,
               std::string &immediate_uppercase,
               std::string &t_file,
               size_t line_number,
               size_t column)
        : Immediate(immediate, immediate_uppercase, t_file, line_number, column)
{
        bool negative = false;

        if (immediate.at(0) == '-') {
                negative = true;
                immediate.erase(0, 1);
        }

        if (immediate.length() > 2 && std::toupper(immediate.at(1)) == 'B') {
                immediate.erase(0, 2);
        } else if (std::toupper(immediate.at(0)) == 'B') {
                immediate.erase(0, 1);
        }

        if (immediate.length() > 16) {
                is_valid = false;
        } else {
                try {
                        const uint64_t v = std::bitset<16>(immediate).to_ullong();
                        if (v & (~0xFFFF)) {
                                is_valid = false;
                        } else {
                                value = static_cast<int16_t>(v);
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
                        "Invalid literal for 16 bit signed base 2 value",
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
        } else {
                if (negative) {
                        value = static_cast<std::int16_t>(-value);
                }
        }
}
