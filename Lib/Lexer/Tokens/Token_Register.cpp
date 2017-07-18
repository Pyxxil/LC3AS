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
            Diagnostics::FileContext(file, at_line, at_column),
            "Invalid register", Diagnostics::SPELLING, Diagnostics::ERROR
        );

        diag.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(file, at_line, at_column),
                    '^', "A valid register lies between R0 and R7 (inclusive)", lexed_lines[file].at(at_line)
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
