#include "Tokens/Immediates/Token_Immediate_Binary.hpp"

#if defined(_MSC_VER)
#include <cctype>
#endif

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Binary::Binary(std::string& immediate,
               std::string& immediate_uppercase,
               std::string& t_file,
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
    value = static_cast<int16_t>(std::bitset<16>(immediate).to_ulong());
  }

  if (!is_valid) {
    Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, column),
      "Invalid literal for 16 bit signed base 2 value",
      Diagnostics::INVALID_LITERAL,
      Diagnostics::ERROR);

    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
        Diagnostics::FileContext(file, line, column),
        '^',
        "Found here",
        lexed_lines[file].at(line)),
      '~',
      token.length()));

    Diagnostics::push(diagnostic);
  } else if (negative) {
    value = -value;
  }
}
