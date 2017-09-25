#include "Tokens/Immediates/Token_Immediate_Octal.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Octal::Octal(std::string& immediate,
             const std::string& t_file,
             size_t line_number,
             size_t t_column)
  : Immediate(immediate, immediate, t_file, line_number, t_column)
{
  char* check = nullptr;
  const auto v = std::strtol(immediate.c_str(), &check, 8);

  if (nullptr == check || immediate.c_str() == check ||
      v > std::numeric_limits<int16_t>::max() ||
      v < std::numeric_limits<int16_t>::min()) {
    is_valid = false;
    // TODO: Provide helpful fix-its here if possible (e.g. think of
    // changing to base 10/16, etc.)
    Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, t_column),
      "Invalid literal for 16 bit signed base 8 value",
      Diagnostics::INVALID_LITERAL,
      Diagnostics::ERROR);

    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
        Diagnostics::FileContext(file, line, t_column),
        '^',
        "Found here",
        lexed_lines[file][line]),
      '~',
      token.length()));

    Diagnostics::push(diagnostic);
  } else {
    value = static_cast<std::int16_t>(v);
  }
}
