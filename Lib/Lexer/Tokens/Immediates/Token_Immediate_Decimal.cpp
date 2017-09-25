#include "Tokens/Immediates/Token_Immediate_Decimal.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Decimal::Decimal(std::string& immediate,
                 const std::string& t_file,
                 size_t line_number,
                 size_t t_column)
  : Immediate(immediate, immediate, t_file, line_number, t_column)
{
  if (immediate.length() > 7) {
    is_valid = false;
  } else {
    if ('#' == immediate.front()) {
      immediate.erase(0, 1);
    }

    if (immediate.length() > 0) {
      char* check = nullptr;
      const auto v = std::strtol(immediate.c_str(), &check, 10);

      if (nullptr == check || v > std::numeric_limits<int16_t>::max() ||
          v < std::numeric_limits<int16_t>::min()) {
        is_valid = false;
      } else {
        value = static_cast<std::int16_t>(v);
        return; // We're done here
      }
    } else {
      // TODO: Should this be a warning, and just set the value to 0?
      // This is mostly for cases where the user puts a single '#' on a line
      is_valid = false;
    }
  }

  // We should only reach here if the conversion was unsuccessful
  Diagnostics::Diagnostic diagnostic(
    Diagnostics::FileContext(file, line, t_column),
    "Invalid literal for 16 bit signed base 10 value",
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
}
