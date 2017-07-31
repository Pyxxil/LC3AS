#include <Tokens/Token_Immediate.hpp>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Immediate::Immediate(std::string& immediate,
                     std::string& immediate_uppercase,
                     std::string& t_file,
                     size_t line_number,
                     size_t t_column)
  : Token(immediate, immediate_uppercase, t_file, line_number, t_column)
{
}

void
Immediate::requires_too_many_bits(int allowed_bits,
                                  bool is_signed,
                                  const Token* const caller,
                                  const std::map<std::string, Symbol>& symbols)
{
  (void)symbols;
  (void)caller;

  Diagnostics::Diagnostic diagnostic(
    Diagnostics::FileContext(file, line, column),
    "Address too far away.",
    Diagnostics::INVALID_LABEL,
    Diagnostics::ERROR);

  std::stringstream error_string;
  error_string << "Immediate value '" << token
               << "' can not be represented as a " << allowed_bits << " bit "
               << (is_signed ? "signed" : "unsigned") << " value";

  diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
    Diagnostics::SelectionContext(Diagnostics::FileContext(file, line, column),
                                  '^',
                                  error_string.str(),
                                  lexed_lines[file].at(line)),
    '~',
    token.length()));

  Diagnostics::push(diagnostic);
}

Token::token_type
Immediate::type() const
{
  return Token::token_type::IMMEDIATE;
}
