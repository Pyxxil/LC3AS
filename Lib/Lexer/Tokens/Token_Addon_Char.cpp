#include "Tokens/Token_Addon_Char.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Character::Character(std::string &character,
                     std::string &t_file,
                     size_t line_number,
                     size_t column)
        : Immediate(character, character, t_file, line_number, column)
{
        if (character.length() > 1) {
                if (character.length() == 2 && character.at(0) == '\\') {
                        switch (character.at(1)) {
                        case '\\':
                                value = '\\';
                                return;
                        case 'n':
                                value = '\n';
                                return;
                        case 't':
                                value = '\t';
                                return;
                        case '\'':
                                value = '\'';
                                return;
                        case '0':
                                value = 0;
                                break;
                        default:
                                break;
                        }
                } else {
                        is_valid = false;

                        Diagnostics::Diagnostic diag(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(
                                                file,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                line_number,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                column,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        )
                                ),
                                "Invalid character literal",
                                Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                                Diagnostics::DIAGNOSTIC::ERROR
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
                                                                line_number,
                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                        ),
                                                        Diagnostics::Variant<size_t>(
                                                                column,
                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                        )
                                                ), '^', "Found here",lexed_lines[file].at(line_number - 1)
                                        ), '~', character.length()
                                )
                        );

                        Diagnostics::push(diag);
                }
        } else if (0u == character.length()) {
                Diagnostics::Diagnostic diag(
                        Diagnostics::FileContext(
                                Diagnostics::Variant<std::string>(
                                        file,
                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                Diagnostics::Variant<size_t>(
                                        line_number,
                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                Diagnostics::Variant<size_t>(
                                        column,
                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                )
                        ),
                        "Invalid character literal",
                        Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                        Diagnostics::DIAGNOSTIC::ERROR
                );

                diag.provide_context(
                        std::make_unique<Diagnostics::SelectionContext>(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(
                                                file,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                line_number,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                column,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        )
                                ),
                                '^',
                                "Expected character, not empty sequence",
                                lexed_lines[file].at(line_number - 1)
                        )
                );

                Diagnostics::push(diag);
        } else {
                value = static_cast<std::int16_t>(character.at(0));
        }
}
