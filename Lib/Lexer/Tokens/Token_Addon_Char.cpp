#include "Tokens/Token_Addon_Char.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Character::Character(std::string &character, std::string &t_file,
                     size_t line_number, size_t column)
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

            Diagnostics::Diagnostic diagnostic(
                Diagnostics::FileContext(file, line_number, column),
                "Invalid character literal",
                Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                Diagnostics::DIAGNOSTIC::ERROR);

            diagnostic.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                    Diagnostics::SelectionContext(
                        Diagnostics::FileContext(file, line_number, column),
                        '^', "Found here", lexed_lines[file].at(line_number)),
                    '~', character.length()));

            Diagnostics::push(diagnostic);
        }
    } else if (0u == character.length()) {
        Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file, line_number, column),
            "Invalid character literal", Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
            Diagnostics::DIAGNOSTIC::ERROR);

        diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
                Diagnostics::FileContext(file, line_number, column), '^',
                "Expected character, not empty sequence",
                lexed_lines[file].at(line_number)));

        Diagnostics::push(diagnostic);
    } else {
        value = static_cast<std::int16_t>(character.at(0));
    }
}
