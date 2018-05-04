#include "Tokens/Immediates/Token_Immediate_Hexadecimal.hpp"

#include <limits>
#if defined(_MSC_VER)
#include <cctype>
#endif

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Hexadecimal::Hexadecimal(std::string &immediate,
                         const std::string &immediate_uppercase,
                         const std::string &t_file, size_t line_number,
                         size_t t_column)
    : Immediate(immediate, immediate_uppercase, t_file, line_number, t_column) {
  if (immediate.length() > 6 || immediate.length() == 1 ||
      (immediate.length() == 2 && 'X' == immediate_uppercase[1])) {
    is_valid = false;
  } else {
    if ('X' == immediate_uppercase.front()) {
      immediate.insert(0, 1, '0');
    }

    if (immediate[2] == '-') {
      immediate.erase(immediate.begin() + 2);
      immediate.insert(0, 1, '-');
    }

    char *check = nullptr;
    const auto v = std::strtol(immediate.c_str(), &check, 16);
    if (nullptr == check || v > (2 * std::numeric_limits<int16_t>::max()) + 1) {
      is_valid = false;
    } else {
      value = static_cast<int16_t>(v);
      return; // We're done here
    }
  }

  Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, t_column),
      "Invalid literal for 16 bit signed base 16 value",
      Diagnostics::INVALID_LITERAL, Diagnostics::ERROR);

  diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
          Diagnostics::FileContext(file, line, t_column), '^', "Found here",
          lexed_lines[file][line]),
      '~', token.length()));

  Diagnostics::push(diagnostic);
}
