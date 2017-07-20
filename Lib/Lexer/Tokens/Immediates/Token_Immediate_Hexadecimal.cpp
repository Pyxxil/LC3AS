#include "Tokens/Immediates/Token_Immediate_Hexadecimal.hpp"

#include <limits>
#if defined(_WIN64)
#include <cctype>
#endif

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Hexadecimal::Hexadecimal(std::string& immediate,
                         std::string& immediate_uppercase,
                         std::string& t_file,
                         size_t line_number,
                         size_t column)
  : Immediate(immediate, immediate_uppercase, t_file, line_number, column)
{
  if (immediate.length() > 6 || immediate.length() == 1 ||
      (immediate.length() == 2 && std::toupper(immediate.at(1)) == 'X')) {
    is_valid = false;
  } else {

    if (std::toupper(immediate.at(0)) == 'X') {
      immediate.insert(0, 1, '0');
    }

    char* check = nullptr;
    const auto v = std::strtol(immediate.c_str(), &check, 16);
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
      "Invalid literal for 16 bit signed base 16 value",
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
