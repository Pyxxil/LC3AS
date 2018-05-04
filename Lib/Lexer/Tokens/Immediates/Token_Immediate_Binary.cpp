#include "Tokens/Immediates/Token_Immediate_Binary.hpp"

#if defined(_MSC_VER)
#include <cctype>
#endif

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Binary::Binary(std::string &immediate, const std::string &immediate_uppercase,
               const std::string &t_file, size_t line_number, size_t t_column)
    : Immediate(immediate, immediate_uppercase, t_file, line_number, t_column) {
  bool negative = false;

  if ('-' == immediate.front()) {
    negative = true;
    immediate.erase(0, 1);
  }

  if (immediate.length() > 2 && 'B' == immediate_uppercase[1]) {
    immediate.erase(0, 2);
  } else if ('B' == immediate_uppercase.front()) {
    immediate.erase(0, 1);
  }

  if (immediate.front() == '-') {
    negative = true;
    immediate.erase(0, 1);
  }

  if (immediate.length() > 16) {
    is_valid = false;
  } else {
    value = negative
                ? -static_cast<int16_t>(std::bitset<16>(immediate).to_ulong())
                : static_cast<int16_t>(std::bitset<16>(immediate).to_ulong());
    return; // We're done here
  }

  Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, t_column),
      "Invalid literal for 16 bit signed base 2 value",
      Diagnostics::INVALID_LITERAL, Diagnostics::ERROR);

  diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
          Diagnostics::FileContext(file, line, t_column), '^', "Found here",
          lexed_lines[file][line]),
      '~', token.length()));

  Diagnostics::push(diagnostic);
}
