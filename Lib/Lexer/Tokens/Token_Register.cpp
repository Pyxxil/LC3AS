#include "Tokens/Token_Register.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Register::Register(std::string &which,
                   std::string &which_uppercase,
                   std::string &t_file,
                   size_t line_number,
                   size_t column)
        : Token(which, which_uppercase, t_file, line_number, column), reg(which.at(1) - '0')
{
        // Valid registers are R0, R1, R2, R3, R4, R5, R6, and R7
        if (reg > 7 || reg < 0) {
                // TODO: Use Diagnostics here for a better error message
                // TODO: Use a highlight context, with the fix_it being "R{0..7}"
                Diagnostics::Diagnostic diag(
                        Diagnostics::FileContext(
                                Diagnostics::Variant<std::string>(file, Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<size_t>(at_line, Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<size_t>(at_column, Console::FOREGROUND_COLOUR::YELLOW)
                        ), "Invalid register", Diagnostics::SPELLING, Diagnostics::ERROR
                );

                diag.provide_context(
                        std::make_unique<Diagnostics::HighlightContext>(
                                Diagnostics::SelectionContext(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(file, Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(at_line, Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(at_column, Console::FOREGROUND_COLOUR::YELLOW)
                                        ), '^', "A valid register lies between 0..7", std::string(lexed_lines[file].at(at_line - 1))
                                ), '~', token.length(), "R{0..7}"
                        )
                );

                Diagnostics::push(diag);
        }
}

Token::token_type Register::type() const
{
        return Token::token_type::REGISTER;
}
