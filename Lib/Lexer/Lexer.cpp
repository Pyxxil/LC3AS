#include "Lexer.hpp"

#include <regex>

#if defined(_MSC_VER)
// Windows doesn't like not having this here apparently.
#include <cctype>
#endif

#include "LexHelper.hpp"
#include "Tokens/Tokens.hpp"

std::map<std::string, std::vector<std::string>> lexed_lines;

std::vector<std::string> open_files;

static constexpr size_t hashed_letters[] = {
  100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997, 66457,
  65719,  70957, 50262, 48407, 51151, 41047, 39371, 35401, 37039,
  28697,  27791, 20201, 21523, 6449,  4813,  16333, 13337,
};

/*! Create a hash of a c-string.
 *
 * @param string The c-string to hash.
 * @param length The length of the c-string.
 *
 * @return The hash.
 */
static constexpr size_t
hash(const char* const string, size_t length)
{
  size_t _hash{ 37 };

  for (size_t index = 0; index < length; ++index) {
    _hash =
      (_hash *
       hashed_letters[(static_cast<size_t>(string[index]) - 0x41u) % 26]) ^
      (static_cast<size_t>(*string) *
       hashed_letters[(static_cast<size_t>(string[index]) - 0x41u) % 26]);
  }

  return _hash;
}

/*! The same as above, just at run time and with std::string objects
 *
 * @param string The string to hash
 *
 * @return The hash of the string
 */
static size_t
hash(const std::string& string)
{
  // Basically, we don't really want something that's likely to be an
  // immediate value, or a label (less likely to be caught here, but may as
  // well try).
  if (string.at(0) != '.' && (std::isprint(string.at(0)) == 0)) {
    return 0;
  }

  size_t _hash{ 37 };

  for (const auto character : string) {
    _hash =
      (_hash * hashed_letters[(static_cast<size_t>(character) - 0x41u) % 26]) ^
      (static_cast<size_t>(string.at(0)) *
       hashed_letters[(static_cast<size_t>(character) - 0x41u) % 26]);
  }

  return _hash;
}

void
Lexer::lex(std::vector<std::vector<std::shared_ptr<Token>>>& t_tokens,
           std::map<std::string, Symbol>& t_symbols)
{
  if (is_fail) {
    return;
  }

  std::string t_line;

  lexed_lines.insert(std::make_pair<std::string, std::vector<std::string>>(
    std::string(file_name), { std::string() }));

  while (std::getline(file, t_line)) {
    lexed_lines[file_name].emplace_back(t_line);
  }

  std::vector<std::shared_ptr<Token>> tokenized_line;
  size_t line_number{ 0 };

  for (const auto& line_entry : lexed_lines[file_name]) {
    tokenizeLine(line_entry, line_number, tokenized_line);

    if (!tokenized_line.empty()) {
#ifdef INCLUDE_ADDONS
      if (tokenized_line.front()->type() == Token::ADDON_INCLUDE) {
        if (tokenized_line.front()->valid_arguments(tokenized_line)) {
          const auto&& file_with_path =
            file_name.substr(0, file_name.find_last_of('/') + 1) +
            tokenized_line.back()->token;

          auto&& file_is_already_open =
            std::find_if(open_files.cbegin(),
                         open_files.cend(),
                         [&file_with_path](const auto& _file) -> bool {
                           return _file == file_with_path;
                         }) != open_files.cend();

          if (file_is_already_open) {
            Diagnostics::Diagnostic diagnostic(
              Diagnostics::FileContext(file_name,
                                       tokenized_line.front()->line,
                                       tokenized_line[1]->column),
              "Recursive include detected.",
              Diagnostics::LOGIC,
              Diagnostics::ERROR);

            diagnostic.provide_context(
              std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                  Diagnostics::FileContext(file_name,
                                           tokenized_line.front()->line,
                                           tokenized_line[1]->column),
                  '^',
                  "In include directive found here",
                  lexed_lines[file_name].at(tokenized_line.front()->line)),
                '~',
                tokenized_line[1]->token.length()));

            provide_context(diagnostic);
            Diagnostics::push(diagnostic);
          } else {
            std::vector<std::vector<std::shared_ptr<Token>>> _tokens;
            std::map<std::string, Symbol> _symbols;

            Lexer lexer(this,
                        file_with_path,
                        tokenized_line.front()->line,
                        tokenized_line[1]->column,
                        tokenized_line[1]->token.length());
            lexer.lex(_tokens, _symbols);

            tokens.insert(tokens.end(), _tokens.begin(), _tokens.end());
            symbols.insert(_symbols.begin(), _symbols.end());
          }
        }
      } else {
        tokens.emplace_back(tokenized_line);
      }
#else
      tokens.push_back(tokenized_line);
#endif
      tokenized_line.clear();
    }

    ++line_number;
  }

  t_tokens = tokens;
  t_symbols = symbols;
}

/*! Tokenize a string.
 *
 * @param word The string to tokenize
 * @param line_number The line number the string was found on
 * @param column The column the string was in in the file
 *
 * @return A shared pointer containing everything needed to know about the token
 */
std::shared_ptr<Token>
Lexer::tokenize(std::string word, size_t line_number, size_t t_column)
{
  std::string copy{ word };
  std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

  t_column -= word.length();

  // TODO: While this makes it a bit more efficient, is it worth double
  // TODO: checking that the strings are the same after comparing the hash's?
  // TODO: Just as a precautionary measure.

  // This is non-zero if the string contains some letters.
  const size_t hashed = hash(copy);

  if (0u != hashed) {
    switch (hashed) {
      case hash("ADD", 3):
        return std::make_shared<Add>(
          word, copy, file_name, line_number, t_column);
      case hash("AND", 3):
        return std::make_shared<And>(
          word, copy, file_name, line_number, t_column);
      case hash("NOT", 3):
        return std::make_shared<Not>(
          word, copy, file_name, line_number, t_column);
      case hash("JSR", 3):
        return std::make_shared<Jsr>(
          word, copy, file_name, line_number, t_column);
      case hash("JSRR", 4):
        return std::make_shared<Jsrr>(
          word, copy, file_name, line_number, t_column);
      case hash("JMP", 3):
        return std::make_shared<Jmp>(
          word, copy, file_name, line_number, t_column);
      case hash("RET", 3):
        return std::make_shared<Ret>(
          word, copy, file_name, line_number, t_column);
      case hash("ST", 2):
        return std::make_shared<St>(
          word, copy, file_name, line_number, t_column);
      case hash("STR", 3):
        return std::make_shared<Str>(
          word, copy, file_name, line_number, t_column);
      case hash("STI", 3):
        return std::make_shared<Sti>(
          word, copy, file_name, line_number, t_column);
      case hash("LD", 2):
        return std::make_shared<Ld>(
          word, copy, file_name, line_number, t_column);
      case hash("LEA", 3):
        return std::make_shared<Lea>(
          word, copy, file_name, line_number, t_column);
      case hash("LDI", 3):
        return std::make_shared<Ldi>(
          word, copy, file_name, line_number, t_column);
      case hash("LDR", 3):
        return std::make_shared<Ldr>(
          word, copy, file_name, line_number, t_column);
      case hash("PUTS", 4):
        return std::make_shared<Puts>(
          word, copy, file_name, line_number, t_column);
      case hash("PUTSP", 5):
        return std::make_shared<Putsp>(
          word, copy, file_name, line_number, t_column);
      case hash("HALT", 4):
        return std::make_shared<Halt>(
          word, copy, file_name, line_number, t_column);
      case hash("TRAP", 4):
        return std::make_shared<Trap>(
          word, copy, file_name, line_number, t_column);
      case hash("GETC", 4):
        return std::make_shared<Getc>(
          word, copy, file_name, line_number, t_column);
      case hash("OUT", 3): // FALLTHROUGH
      case hash("PUTC", 4):
        return std::make_shared<Out>(
          word, copy, file_name, line_number, t_column);
      case hash("IN", 2):
        return std::make_shared<In>(
          word, copy, file_name, line_number, t_column);
      case hash("BR", 2):    // FALLTHROUGH
      case hash("BRNZP", 5): // FALLTHROUGH
      case hash("BRNPZ", 5): // FALLTHROUGH
      case hash("BRZNP", 5): // FALLTHROUGH
      case hash("BRZPN", 5): // FALLTHROUGH
      case hash("BRPNZ", 5): // FALLTHROUGH
      case hash("BRPZN", 5):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, true, true, true);
      case hash("BRN", 3):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, true, false, false);
      case hash("BRZ", 3):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, false, true, false);
      case hash("BRP", 3):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, false, false, true);
      case hash("BRNZ", 4): // FALLTHROUGH
      case hash("BRZN", 4):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, true, true, false);
      case hash("BRNP", 4): // FALLTHROUGH
      case hash("BRPN", 4):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, true, false, true);
      case hash("BRZP", 4): // FALLTHROUGH
      case hash("BRPZ", 4):
        return std::make_shared<Br>(
          word, copy, file_name, line_number, t_column, false, true, true);
      case hash(".ORIG", 5):
        return std::make_shared<Orig>(
          word, copy, file_name, line_number, t_column);
      case hash(".END", 4):
        return std::make_shared<End>(
          word, copy, file_name, line_number, t_column);
      case hash(".FILL", 5):
        return std::make_shared<Fill>(
          word, copy, file_name, line_number, t_column);
      case hash(".BLKW", 5):
        return std::make_shared<Blkw>(
          word, copy, file_name, line_number, t_column);
      case hash(".STRINGZ", 8):
        return std::make_shared<Stringz>(
          word, copy, file_name, line_number, t_column);
#ifdef INCLUDE_ADDONS
      case hash(".NEG", 4):
        return std::make_shared<Neg>(
          word, copy, file_name, line_number, t_column);
      case hash(".SUB", 4):
        return std::make_shared<Sub>(
          word, copy, file_name, line_number, t_column);
      case hash(".SET", 4):
        return std::make_shared<Set>(
          word, copy, file_name, line_number, t_column);
      case hash(".LSHIFT", 7):
        return std::make_shared<Lshift>(
          word, copy, file_name, line_number, t_column);
      case hash(".INCLUDE", 8):
        return std::make_shared<Include>(
          word, copy, file_name, line_number, t_column);
#endif
      default:
        break;
    }
  }

  static const std::regex decimal("#?-?\\d+", std::regex_constants::optimize);
  static const std::regex binary("-?0?[bB][01]+",
                                 std::regex_constants::optimize);
  static const std::regex hexadecimal("0?[xX][\\da-fA-F]+",
                                      std::regex_constants::optimize);
  static const std::regex octal("\\\\[0-7]+", std::regex_constants::optimize);
  static const std::regex _register("[rR]\\d", std::regex_constants::optimize);
  static const std::regex label("\\.?[\\da-zA-Z_]+",
                                std::regex_constants::optimize);

  if (std::regex_match(word, _register)) {
    return std::make_shared<Register>(
      word, copy, file_name, line_number, t_column);
  }

  if (std::regex_match(word, decimal)) {
    return std::make_shared<Decimal>(word, file_name, line_number, t_column);
  }

  if (std::regex_match(word, binary)) {
    return std::make_shared<Binary>(
      word, copy, file_name, line_number, t_column);
  }

  if (std::regex_match(word, hexadecimal)) {
    return std::make_shared<Hexadecimal>(
      word, copy, file_name, line_number, t_column);
  }

  if (std::regex_match(word, octal)) {
    return std::make_shared<Octal>(word, file_name, line_number, t_column);
  }

  if (std::regex_match(word, label)) {
    return std::make_shared<Label>(word, file_name, line_number, t_column);
  }

  auto&& token =
    std::make_shared<Token>(word, copy, file_name, line_number, t_column);
  token->is_valid = false;
  return token;
}

/*! Tokenize a single line from the file
 *
 * Go through each string (terminated by ',', space character (as defined by
 * std::isspace), or a comment (denoted by ';', "//"), and tokenize it, adding
 * it to a vector until the end of the line is reached. The end of a line is
 * determined when either a comment is hit, or the end of the line is reached
 * (that is, when the current index into the string is >= to the length of the
 * string).
 *
 * @param line The line to tokenize
 * @param line_number The current line number. This is only relevant when
 * dealing with files, so defaults to 0.
 * @param into The vector to tokenize the line into
 */
void
Lexer::tokenizeLine(std::string t_line,
                    size_t line_number,
                    std::vector<std::shared_ptr<Token>>& into)
{
  std::string current;

  char terminated_by{ 0 };
  size_t index{ 0 };

  for (; index < t_line.length();) {
    char character{ t_line.at(index) };

    // We don't care about space characters.
    if (0 != std::isspace(character)) {
      terminated_by = ' ';

      const size_t col = index;
      while (t_line.length() > index + 1 && 0 != std::isspace(character)) {
        character = t_line.at(++index);
      }

      if (!current.empty()) {
        // We terminated with something, so we want to remember that.
        // This helps in cases like this:
        // ADD R0 , R0 , R1
        // Which, without this, would make 2 warnings (one for each
        // comma)
        terminated_by = 1;
        addToken(current, into, line_number, col);
      }
    }

    switch (character) {
      case ';': {
        // Hit a comment
        goto end_of_line;
      }
      case '\r': {
        // std::getline doesn't consume '\r' (at least on OSX)
        goto end_of_line;
      }
      case '/': {
        // '//' is a comment as well.
        if (index + 1 >= t_line.length() || t_line.at(index + 1) != '/') {
          // It seems easiest to treat it as a comment anyways, as '/'
          // can't be used for anything.
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file_name, line_number, index),
            "Treating this as a comment",
            Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
            Diagnostics::DIAGNOSTIC::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(file_name, line_number, index),
              '^',
              "Found unexpected '/'; Did you mean '//'?",
              t_line,
              "//"));

          Diagnostics::push(diagnostic);
        }

        goto end_of_line;
      }
      case ',': {
        if (into.empty() ||
            (' ' != terminated_by && 0 != terminated_by &&
             1 != terminated_by) ||
            (0u == current.length() && terminated_by != 1)) {
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file_name, line_number, index),
            "Extraneous comma",
            Diagnostics::SYNTAX,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(file_name, line_number, index),
              '^',
              "Found here",
              t_line));

          Diagnostics::push(diagnostic);
          terminated_by = ',';
        }

        addToken(current, into, line_number, index);
        break;
      }
      case ':': {
        if (!into.empty() || 0u == current.length() || 0 != terminated_by) {
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file_name, line_number, index),
            "Extraneous colon",
            Diagnostics::SYNTAX,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(file_name, line_number, index),
              '^',
              "Found here",
              t_line));

          Diagnostics::push(diagnostic);
          terminated_by = ':';
        }

        addToken(current, into, line_number, index);
        break;
      }
#ifdef INCLUDE_ADDONS
      case '\'':
#endif
      case '"': {
        addToken(current, into, line_number, index);

        const char terminator{ character };
        while (index + 1 < t_line.length()) {
          character = t_line.at(++index);
          if (character == '\\' && t_line.length() > index + 1 &&
              t_line.at(index + 1) == terminator) {
            character = t_line.at(++index);
          } else if (character == terminator) {
            break;
          }
          current += character;
        }

        if (character != terminator) {
          // TODO: Use Diagnostics here
          std::stringstream stream;
#ifdef INCLUDE_ADDONS
          stream << "Unterminated "
                 << (terminator == '\'' ? "character" : "string");
#else
          stream << "Unterminated string";
#endif
          into.clear();

          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(
              file_name, line_number, index - current.size()),
            stream.str(),
            Diagnostics::SYNTAX,
            Diagnostics::ERROR);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              // Has to be index + 1 because otherwise it'll select
              // the last character
              Diagnostics::FileContext(file_name, line_number, index + 1),
              '^',
              "Expected '" + std::string({ terminator }) +
                "' before end of line.",
              t_line,
              std::string({ terminator })));

          Diagnostics::push(diagnostic);
#ifdef INCLUDE_ADDONS
        } else if ('\'' == terminator) {
          into.push_back(std::make_shared<Character>(
            current, file_name, line_number, index - current.length()));
#endif
        } else {
          into.push_back(std::make_shared<String>(
            current, file_name, line_number, index - current.length()));
        }
        current.erase();
        break;
      }
      default: {
        current += character;
        terminated_by = 0;
        break;
      }
    }
    ++index;
  }
end_of_line:
  addToken(current, into, line_number, index);
}

void
Lexer::provide_context(Diagnostics::Diagnostic& diagnostic)
{
  if (nullptr != parent) {
    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
        Diagnostics::FileContext(parent->file_name, line, column),
        '^',
        "In file included from here",
        lexed_lines[parent->file_name].at(line)),
      '~',
      length));

    parent->provide_context(diagnostic);
  }
}

Lexer::Lexer(const std::string& t_file)
  : line(0)
  , column(0)
  , length(0)
  , file_name(t_file)
  , file(t_file)
  , symbols()
  , parent(nullptr)
  , tokens()
{
  open_files.emplace_back(file_name);
}

Lexer::Lexer(Lexer* const t_parent,
             const std::string& t_file,
             size_t t_line,
             size_t t_col,
             size_t len)
  : line(t_line)
  , column(t_col)
  , length(len)
  , file_name(t_file)
  , file(t_file)
  , symbols()
  , parent(t_parent)
  , tokens()
{
  if (file.fail()) {
    is_fail = true;
    Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file_name, line, t_col),
      "Unable to open file",
      Diagnostics::INVALID_FILE,
      Diagnostics::ERROR);

    provide_context(diagnostic);

    Diagnostics::push(diagnostic);
  }

  open_files.emplace_back(file_name);
}

Lexer::~Lexer()
{
  open_files.erase(
    std::find_if(open_files.begin(), open_files.end(), [this](auto&& _file) {
      return _file == file_name;
    }));
}
