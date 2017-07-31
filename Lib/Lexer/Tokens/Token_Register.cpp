#include "Tokens/Token_Register.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Register::Register(std::string& which,
                   std::string& which_uppercase,
                   std::string& t_file,
                   size_t line_number,
                   size_t t_column)
  : Token(which, which_uppercase, t_file, line_number, t_column)
  , reg(which.at(1) - '0')
{
  // Valid registers are R0, R1, R2, R3, R4, R5, R6, and R7
  if (reg > 7 || reg < 0) {
    Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, t_column),
      "Invalid register",
      Diagnostics::SPELLING,
      Diagnostics::ERROR);

    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
        Diagnostics::FileContext(file, line, t_column),
        '^',
        "A valid register lies between R0 and R7 (inclusive)",
        lexed_lines[file].at(line)),
      '~',
      token.length(),
      "R{0..7}"));

    Diagnostics::push(diagnostic);
  }
}

Token::token_type
Register::type() const
{
  return Token::token_type::REGISTER;
}
