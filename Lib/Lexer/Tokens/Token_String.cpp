#include "Tokens/Tokens.hpp"

#include <sstream>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

String::String(std::string& string,
               std::string& t_file,
               size_t line_number,
               size_t t_column)
  : Token(string, string, t_file, line_number, t_column)
{
  for (size_t index = 0; index < string.length(); ++index) {
    if (string.at(index) == '\\') {
      if (index + 1 < string.length()) {
        switch (string.at(index + 1)) {
          case 'n':
            ++index;
            assembled.emplace_back('\n');
            break;
          case 't':
            ++index;
            assembled.emplace_back('\t');
            break;
          case 'v':
            ++index;
            assembled.emplace_back('\v');
            break;
          case '"':
            ++index;
            assembled.emplace_back('"');
            break;
          case '\\':
            assembled.emplace_back('\\');
            break;
          default:
            Diagnostics::Diagnostic diagnostic(
              Diagnostics::FileContext(t_file, line_number, t_column + index),
              "Invalid character escape sequence ('\\" +
                std::string({ string.at(index + 1) }) + "'). Ignoring the '\\'",
              Diagnostics::SYNTAX,
              Diagnostics::WARNING);

            diagnostic.provide_context(
              std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                  Diagnostics::FileContext(
                    t_file, line_number, t_column + index),
                  '^',
                  "Escape sequence found here",
                  lexed_lines[t_file].at(line_number)),
                '~',
                2));

            Diagnostics::push(diagnostic);
            assembled.emplace_back(string.at(index + 1));
            break;
        }
      } else {
        Diagnostics::Diagnostic diagnostic(
          Diagnostics::FileContext(t_file, line_number, t_column + index),
          "Unended escape sequence. Using '\'",
          Diagnostics::SYNTAX,
          Diagnostics::WARNING);

        Diagnostics::push(diagnostic);
        assembled.emplace_back('\\');
      }
    } else {
      assembled.emplace_back(static_cast<uint16_t>(string.at(index)));
    }
  }

  assembled.emplace_back('\0');
}

Token::token_type
String::type() const
{
  return Token::_STRING;
}
