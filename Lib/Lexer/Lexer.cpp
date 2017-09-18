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
  100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997, 66457, 65719, 70957,
  50262,  48407, 51151, 41047, 39371, 35401, 37039, 28697, 27791, 20201, 21523,
  6449,   4813,  16333, 13337, 3571,  5519,  26783, 71471, 68371, 104729
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
  size_t hashed{ 37 };
  const size_t first_character = static_cast<size_t>(*string);

  for (size_t index = 0; index < length; ++index) {
    const size_t as_hashed{
      hashed_letters[(static_cast<size_t>(string[index]) - 0x41u) & 0x1F]
    };
    hashed = (hashed * as_hashed) ^ (first_character * as_hashed);
  }

  return hashed;
}

/*! The same as above, just at run time and with std::string objects
 *
 * It should be relatively safe in regards to repeat hashes, as it reduces the
 * number of things that are valid with the two if statements. However, I'm
 * still unsure whether it's smarter to also check if they're the same thing
 * afterwards.
 *
 * @param string The string to hash
 *
 * @return The hash of the string
 */
static size_t
hash(const std::string& string)
{
  const size_t first_character = static_cast<size_t>(string.front());
  if (first_character != '.' &&
      (0 == std::isprint(static_cast<int>(first_character)))) {
    // Basically, we don't really want something that's likely to be an
    // immediate value, or a label (less likely to be caught here, but may as
    // well try).
    return 0;
  }

  if (string.length() > 8 || string.length() < 2) {
    // There are no registers, directives, or instructions which are longer than
    // 8 characters or less than 2, so this string isn't one of them.
    return 0;
  }

  size_t hashed{ 37 };

  for (const auto character : string) {
    const size_t as_hashed{
      hashed_letters[(static_cast<size_t>(character) - 0x41u) & 0x1F]
    };
    hashed = (hashed * as_hashed) ^ (first_character * as_hashed);
  }

  return hashed;
}

/*! Include a given file (via token) into the current tokens
 *
 * @param t_line The tokens relevant to the include file.
 */
void
Lexer::include_file(std::vector<std::shared_ptr<Token>>& t_line)
{
  if (t_line.front()->valid_arguments(t_line)) {
    const auto&& file_with_path =
      file_name.substr(0, file_name.find_last_of('/') + 1) +
      t_line.back()->token;

    const bool file_is_already_open =
      std::find_if(open_files.cbegin(),
                   open_files.cend(),
                   [&file_with_path](const auto& _file) -> bool {
                     return _file == file_with_path;
                   }) != open_files.cend();

    if (file_is_already_open) {
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(
          file_name, t_line.front()->line, t_line[1]->column),
        "Recursive include detected.",
        Diagnostics::LOGIC,
        Diagnostics::ERROR);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(
              file_name, t_line.front()->line, t_line[1]->column),
            '^',
            "In include directive found here",
            lexed_lines[file_name][t_line.front()->line]),
          '~',
          t_line[1]->token.length()));

      provide_context(diagnostic);
      Diagnostics::push(diagnostic);
    } else {
      std::vector<std::vector<std::shared_ptr<Token>>> _tokens;
      std::map<std::string, Symbol> _symbols;

      Lexer lexer(this,
                  file_with_path,
                  t_line.front()->line,
                  t_line[1]->column,
                  t_line[1]->token.length());
      lexer.lex(_tokens, _symbols);

      tokens.insert(tokens.end(), _tokens.begin(), _tokens.end());
      symbols.insert(_symbols.begin(), _symbols.end());
    }
  }
}

/*! Lex this lexers file into a stream of tokens
 *
 * @param t_tokens The stream to lex the tokens into.
 * @param t_symbols The stream to lex any symbols found into.
 */
void
Lexer::lex(std::vector<std::vector<std::shared_ptr<Token>>>& t_tokens,
           std::map<std::string, Symbol>& t_symbols)
{
  if (is_fail) {
    // Don't bother continuing if there have been errors already.
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
    tokenize_line(Line(line_entry), line_number, tokenized_line);

    if (!tokenized_line.empty()) {
#ifdef INCLUDE_ADDONS
      if (tokenized_line.front()->type() == Token::ADDON_INCLUDE) {
        include_file(tokenized_line);
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
Lexer::tokenize(std::string&& word, size_t line_number, size_t t_column)
{
  std::string copy{ word };
  std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

  t_column -= copy.length();

  // TODO: While this makes it a bit more efficient, is it worth double
  // TODO: checking that the strings are the same after comparing the hash's?
  // TODO: Just as a precautionary measure.

  // This is non-zero if the string contains some letters.
  const size_t hashed{ hash(copy) };

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
      case hash("RTI", 3):
        return std::make_shared<Rti>(
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
      case hash("R0", 2): // FALLTHROUGH
      case hash("R1", 2): // FALLTHROUGH
      case hash("R2", 2): // FALLTHROUGH
      case hash("R3", 2): // FALLTHROUGH
      case hash("R4", 2): // FALLTHROUGH
      case hash("R5", 2): // FALLTHROUGH
      case hash("R6", 2): // FALLTHROUGH
      case hash("R7", 2):
        return std::make_shared<Register>(
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
        // TODO: Is this always a label?
        break;
    }
  }
  return tokenize_immediate_or_label(
    std::move(word), std::move(copy), line_number, t_column);
}

/*! Check if the given token is a binary immediate value, a hexadecimal
 * immediate value, or an octal immediate value.
 *
 * @param copy The uppercase copy of the token.
 * @param index The index we want to start the check from
 * @param word The token
 * @param file_name The name of the file the token was found in
 * @param line_number The line the token was found on
 * @param t_column The column the token started on
 *
 * @return The tokenised value
 */
static std::shared_ptr<Token>
check_binary_hex_or_octal(std::string copy,
                          size_t index,
                          std::string word,
                          const std::string& file_name,
                          size_t line_number,
                          size_t t_column)
{
  if (copy.length() > index) {
    switch (copy[index]) {
      case 'B':
        if (std::all_of(copy.begin() + index + 1, copy.end(), [](const auto c) {
              return c == '0' || c == '1';
            })) {
          return std::make_shared<Binary>(
            word, copy, file_name, line_number, t_column);
        }
        break;
      case 'X':
        if (std::all_of(copy.begin() + 3, copy.end(), [](const auto c) {
              return std::isxdigit(c);
            })) {
          return std::make_shared<Hexadecimal>(
            word, copy, file_name, line_number, t_column);
        }
      default:
        break;
    }
  }

  return std::make_shared<Octal>(word, file_name, line_number, t_column);
}
/*! Tokenize a word into an immediate value, label, or invalid token.
 *
 * If tokenize doesn't match anything, then these are the only three
 * possibilites left.
 *
 * @param word The word to tokenise
 * @param copy The upper case variant of the word
 * @param line_number The line the token has been found on
 * @param t_column The column at which the token was first observed
 *
 * @return The token
 */
std::shared_ptr<Token>
Lexer::tokenize_immediate_or_label(std::string word,
                                   std::string copy,
                                   size_t line_number,
                                   size_t t_column)
{
  // TODO: Make this a littler nicer to look at

  if ('#' == copy.front()) {
    // This can only ever be a decimal number
    return std::make_shared<Decimal>(word, file_name, line_number, t_column);
  }

  if ('-' == copy.front() && copy.length() > 1) {
    switch (copy[1]) {
      // For these, there is no reason to check if all of the values are
      // valid base digits. No label can contain '-'
      case 'B':
        if (std::all_of(copy.begin() + 2, copy.end(), [](const auto c) {
              return c == '0' || c == '1';
            })) {
          return std::make_shared<Binary>(
            word, copy, file_name, line_number, t_column);
        }
        break;
      case 'X':
        if (std::all_of(copy.begin() + 2, copy.end(), [](const auto c) {
              return std::isxdigit(c);
            })) {
          return std::make_shared<Hexadecimal>(
            word, copy, file_name, line_number, t_column);
        }
      case '0':
        return check_binary_hex_or_octal(std::move(copy),
                                         2,
                                         std::move(word),
                                         file_name,
                                         line_number,
                                         t_column);
      default:
        if (std::isdigit(copy[1])) {
          return std::make_shared<Decimal>(
            word, file_name, line_number, t_column);
        }
        break;
    }
  } else if (std::isdigit(copy.front())) {
    switch (copy.front()) {
      case '0':
        return check_binary_hex_or_octal(std::move(copy),
                                         1,
                                         std::move(word),
                                         file_name,
                                         line_number,
                                         t_column);
      default:
        return std::make_shared<Decimal>(
          word, file_name, line_number, t_column);
    }
  } else if ('B' == copy.front() && copy.length() > 1) {
    if (std::all_of(copy.begin() + 1, copy.end(), [](const auto c) {
          return c == '0' || c == '1';
        })) {
      return std::make_shared<Binary>(
        word, copy, file_name, line_number, t_column);
    }
  } else if ('X' == copy.front() && copy.length() > 1) {
    if (std::all_of(copy.begin() + 1, copy.end(), [](const auto c) {
          return std::isxdigit(c);
        })) {
      return std::make_shared<Hexadecimal>(
        word, copy, file_name, line_number, t_column);
    }
  }

  const bool is_valid_label =
    std::all_of(word.begin() + static_cast<size_t>(word.front() == '.'),
                word.end(),
                [](const auto c) { return std::isalnum(c) || '_' == c; });
  if (is_valid_label) {
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
 * std::isspace), or a comment (denoted by ';', "//")), and tokenize it,
 * adding it to a vector until the end of the line is reached. The end of a
 * line is determined when either a comment is hit, or the end of the line is
 * reached (that is, when the current index into the string is >= to the
 * length of the string).
 *
 * @param line The line to tokenize
 * @param line_number The current line number. This is only relevant when
 * dealing with files, so defaults to 0.
 * @param into The vector to tokenize the line into
 */
void
Lexer::tokenize_line(Line&& current_line,
                     size_t line_number,
                     std::vector<std::shared_ptr<Token>>& into)
{
  std::string current;
  char terminated_by{ 0 };

  while (!current_line.at_end()) {
    char character = current_line.next();

    // We don't care about space characters.
    if (0 != std::isspace(character)) {
      terminated_by = ' ';

      const size_t col = current_line.index() - 1;
      current_line.skip_while(
        [](const auto&& c) -> bool { return std::isspace(c) != 0; });
      current_line >> character;
      if (0 == character) {
        break;
      }

      if (!current.empty()) {
        // We terminated with something, so we want to remember that.
        // This helps in cases like this:
        // ADD R0 , R0 , R1
        // Which, without this, would make 2 warnings (one for each
        // comma)
        terminated_by = 1;
        add_token(current, into, line_number, col);
      }
    }

    // std::cout << "Working with " << character << '\n';

    switch (character) {
      case ';': { // Hit a comment
        goto end_of_line;
      }
      case '\r': {
        // std::getline doesn't consume '\r' (at least on OSX)
        goto end_of_line;
      }
      case '/': {
        // '//' is a comment as well.
        if (current_line.peek() != '/') {
          // It seems easiest to treat it as a comment anyways, as '/'
          // can't be used for anything.
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(
              file_name, line_number, current_line.index() - 1),
            "Treating this as a comment",
            Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
            Diagnostics::DIAGNOSTIC::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(
                file_name, line_number, current_line.index() - 1),
              '^',
              "Found unexpected '/'; Did you mean '//'?",
              current_line.line(),
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
            Diagnostics::FileContext(
              file_name, line_number, current_line.index() - 1),
            "Extraneous comma",
            Diagnostics::SYNTAX,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(
                file_name, line_number, current_line.index() - 1),
              '^',
              "Found here",
              current_line.line()));

          Diagnostics::push(diagnostic);
          terminated_by = ',';
        }

        add_token(current, into, line_number, current_line.index() - 1);
        break;
      }
      case ':': {
        if (!into.empty() || 0u == current.length() || 0 != terminated_by) {
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(
              file_name, line_number, current_line.index() - 1),
            "Extraneous colon",
            Diagnostics::SYNTAX,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(
                file_name, line_number, current_line.index() - 1),
              '^',
              "Found here",
              current_line.line()));

          Diagnostics::push(diagnostic);
          terminated_by = ':';
        }

        add_token(current, into, line_number, current_line.index() - 1);
        break;
      }
#ifdef INCLUDE_ADDONS
      case '\'':
#endif
      case '"': {
        // TODO: This should actually just throw an error
        // TODO: Note that we will have to push am invalid token to into
        // TODO: just simply so that .STRINGZ won't compile
        // TODO: :: complaining about too many tokens
        if (!current.empty()) {
#ifdef INCLUDE_ADDONS
          if ('\'' != character || current.length() != 1 ||
              '-' != current.front()) {
            add_token(current, into, line_number, current_line.index());
          }
#else
          add_token(current, into, line_number, current_line.index());
#endif
        }

        const auto begin = current_line.index();
        // TODO: Fix this. It isn't aware of escaped terminators.
        current_line.ignore(Line::ESCAPE_SEQUENCE);
        const auto end = current_line.find_next(character);
        current_line.ignore(Line::RESET);

        if (-1u == end) {
          std::stringstream stream;
#ifdef INCLUDE_ADDONS
          stream << "Unterminated "
                 << (character == '\'' ? "character" : "string");
#else
          stream << "Unterminated string";
#endif
          into.clear();

          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(file_name, line_number, begin - 1),
            stream.str(),
            Diagnostics::SYNTAX,
            Diagnostics::ERROR);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              // Has to be index + 1 because otherwise it'll select
              // the last character
              Diagnostics::FileContext(file_name, line_number, begin - 1),
              '^',
              "Expected " + std::string({ character }) + " to match this one",
              current_line.line()));
          Diagnostics::push(diagnostic);
#ifdef INCLUDE_ADDONS
        } else if ('\'' == character) {
          current = current_line.substr(begin, end - 1);
          into.push_back(std::make_shared<Character>(current,
                                                     file_name,
                                                     line_number,
                                                     current_line.index() -
                                                       current.length() - 1));
#endif
        } else {
          current = current_line.substr(begin, end - 1);
          into.push_back(std::make_shared<String>(current,
                                                  file_name,
                                                  line_number,
                                                  current_line.index() -
                                                    current.length() - 1));
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
  }
end_of_line:
  add_token(current, into, line_number, current_line.index());
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
        lexed_lines[parent->file_name][line]),
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
