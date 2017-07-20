#include "Tokens/Immediates/Token_Immediate_Decimal.hpp"

#include <limits>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Decimal::Decimal(std::string& immediate,
                 std::string& t_file,
                 size_t line_number,
                 size_t column)
  : Immediate(immediate, immediate, t_file, line_number, column)
{
  if (immediate.length() > 7) {
    is_valid = false;
  } else {
    if (immediate.at(0) == '#') {
      immediate.erase(0, 1);
    }

    char* check = nullptr;
    const auto v = std::strtol(immediate.c_str(), &check, 10);
    if (check == nullptr || v > std::numeric_limits<int16_t>::max() ||
        v < std::numeric_limits<int16_t>::min()) {
      is_valid = false;
    } else {
      value = static_cast<std::int16_t>(v);
    }
  }

  if (!is_valid) {
    Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, column),
      "Invalid literal for 16 bit signed base 10 value",
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
  }
}
