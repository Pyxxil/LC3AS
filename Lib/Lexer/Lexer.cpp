#include "Lexer.hpp"

#include <regex>
#include <Lib/Includes/Diagnostics.hpp>

#include "Tokens/Tokens.hpp"

static constexpr std::size_t hashed_letters[] = {
        100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997, 66457
        , 65719, 70957, 50262, 48407, 51151, 41047, 39371, 35401, 37039
        , 28697, 27791, 20201, 21523, 6449, 4813, 16333, 13337,
};

/**
 * Where would we be without constexpr...
 *
 * Create a hash of a c-string.
 *
 * @param string The c-string to hash.
 * @param length The length of the c-string.
 * @return The hash.
 */
static constexpr std::size_t hash(const char *const string, std::size_t length)
{
        std::size_t _hash { 37 };

        for (std::size_t index = 0; index < length; ++index) {
                _hash = (_hash * hashed_letters[(static_cast<std::size_t>(string[index]) - 0x41u) % 26]) ^
                        (static_cast<std::size_t>(*string) *
                         hashed_letters[(static_cast<std::size_t>(string[index]) - 0x41u) % 26]);
        }

        return _hash;
}

static std::size_t hash(const std::string &string)
{
        // Basically, we don't really want something that's likely to be an immediate value,
        // or a label (less likely to be caught here, but may as well try).
        if (string.at(0) != '.' && !std::isprint(string.at(0))) {
                return 0;
        }

        std::size_t _hash { 37 };

        for (const auto &character : string) {
                _hash = (_hash * hashed_letters[(static_cast<std::size_t>(character) - 0x41u) % 26]) ^
                        (static_cast<std::size_t>(string.at(0)) *
                         hashed_letters[(static_cast<std::size_t>(character) - 0x41u) % 26]);
        }

        return _hash;
}

void Lexer::lex()
{
        std::string line;

        std::vector<std::shared_ptr<Token>> tokenized_line;

        for (int line_number = 1; std::getline(file, line); line_number++) {
                if (line.empty()) {
                        continue;
                }

                tokenizeLine(line, line_number, tokenized_line);

                if (!tokenized_line.empty()) {
#ifdef INCLUDE_ADDONS
                        if (tokenized_line.front()->type() == Token::ADDON_INCLUDE) {
                                if (tokenized_line.front()->valid_arguments(tokenized_line)) {
                                        // TODO: This should put the base directory of the current file as the base
                                        // TODO: of the file provided here.
                                        // TODO:        - If that file doesn't exist, maybe check the given path?
                                        // TODO:        - Do we now add a '-I/--Include directory' option?
                                        const std::string &file_with_path =
                                                                  file.substr(
                                                                          0, file.find_last_of('/') + 1) +
                                                                  tokenized_line.back()->token;

                                        auto &&file_is_already_open = std::find_if(
                                                open_files.cbegin(), open_files.cend(),
                                                [&file_with_path](const auto &_file) -> bool
                                                {
                                                        return _file == file_with_path;
                                                }
                                        ) != open_files.cend();

                                        if (file_is_already_open) {
                                                Logging::logger->LOG(Logging::ERROR,
                                                                     tokenized_line.front()->at_line,
                                                                     file,
                                                                     "Recursive include.",
                                                                     Logging::NONE);
                                                ++m_error_count;
                                        } else {
                                                Tokenizer lex(file_with_path);
                                                m_error_count += lex.parse_into(into);
                                        }
                                } else {
                                        ++m_error_count;
                                }
                        } else {
                                into.emplace_back(tokenized_line);
                        }
#else
                        tokens.push_back(tokenized_line);
#endif
                        tokenized_line.clear();
                }
        }
}

std::shared_ptr<Token> Lexer::tokenize(std::string &word, int line_number)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        // TODO: While this makes it a bit more efficient, is it worth double checking that
        // TODO: the strings are the same after comparing the hash's? Just as a precautionary
        // TODO: measure.
        const std::size_t hashed = hash(copy); // This is non-zero if the string contains some letters.

        if (hashed) {
                switch (hashed) {
                case hash("ADD", 3):
                        return std::make_shared<Add>(word, copy, file_name, line_number);
                case hash("AND", 3):
                        return std::make_shared<And>(word, copy, file_name, line_number);
                case hash("NOT", 3):
                        return std::make_shared<Not>(word, copy, file_name, line_number);
                case hash("JSR", 3):
                        return std::make_shared<Jsr>(word, copy, file_name, line_number);
                case hash("JSRR", 4):
                        return std::make_shared<Jsrr>(word, copy, file_name, line_number);
                case hash("JMP", 3):
                        return std::make_shared<Jmp>(word, copy, file_name, line_number);
                case hash("RET", 3):
                        return std::make_shared<Ret>(word, copy, file_name, line_number);
                case hash("ST", 2):
                        return std::make_shared<St>(word, copy, file_name, line_number);
                case hash("STR", 3):
                        return std::make_shared<Str>(word, copy, file_name, line_number);
                case hash("STI", 3):
                        return std::make_shared<Sti>(word, copy, file_name, line_number);
                case hash("LD", 2):
                        return std::make_shared<Ld>(word, copy, file_name, line_number);
                case hash("LEA", 3):
                        return std::make_shared<Lea>(word, copy, file_name, line_number);
                case hash("LDI", 3):
                        return std::make_shared<Ldi>(word, copy, file_name, line_number);
                case hash("LDR", 3):
                        return std::make_shared<Ldr>(word, copy, file_name, line_number);
                case hash("PUTS", 4):
                        return std::make_shared<Puts>(word, copy, file_name, line_number);
                case hash("PUTSP", 5):
                        return std::make_shared<Putsp>(word, copy, file_name, line_number);
                case hash("HALT", 4):
                        return std::make_shared<Halt>(word, copy, file_name, line_number);
                case hash("TRAP", 4):
                        return std::make_shared<Trap>(word, copy, file_name, line_number);
                case hash("GETC", 4):
                        return std::make_shared<Getc>(word, copy, file_name, line_number);
                case hash("OUT", 3): // FALLTHROUGH
                case hash("PUTC", 4):
                        return std::make_shared<Out>(word, copy, file_name, line_number);
                case hash("IN", 2):
                        return std::make_shared<In>(word, copy, file_name, line_number);
                case hash("BR", 2):
                        // FALLTHROUGH
                case hash("BRNZP", 5):
                        // FALLTHROUGH
                case hash("BRNPZ", 5):
                        // FALLTHROUGH
                case hash("BRZNP", 5):
                        // FALLTHROUGH
                case hash("BRZPN", 5):
                        // FALLTHROUGH
                case hash("BRPNZ", 5):
                        // FALLTHROUGH
                case hash("BRPZN", 5):
                        return std::make_shared<Br>(word, copy, file_name, line_number, true, true, true);
                case hash("BRN", 3):
                        return std::make_shared<Br>(word, copy, file_name, line_number, true, false, false);
                case hash("BRZ", 3):
                        return std::make_shared<Br>(word, copy, file_name, line_number, false, true, false);
                case hash("BRP", 3):
                        return std::make_shared<Br>(word, copy, file_name, line_number, false, false, true);
                case hash("BRNZ", 4):
                        // FALLTHROUGH
                case hash("BRZN", 4):
                        return std::make_shared<Br>(word, copy, file_name, line_number, true, true, false);
                case hash("BRNP", 4):
                        // FALLTHROUGH
                case hash("BRPN", 4):
                        return std::make_shared<Br>(word, copy, file_name, line_number, true, false, true);
                case hash("BRZP", 4):
                        // FALLTHROUGH
                case hash("BRPZ", 4):
                        return std::make_shared<Br>(word, copy, file_name, line_number, false, true, true);
                case hash(".ORIG", 5):
                        return std::make_shared<Orig>(word, copy, file_name, line_number);
                case hash(".END", 4):
                        return std::make_shared<End>(word, copy, file_name, line_number);
                case hash(".FILL", 5):
                        return std::make_shared<Fill>(word, copy, file_name, line_number);
                case hash(".BLKW", 5):
                        return std::make_shared<Blkw>(word, copy, file_name, line_number);
                case hash(".STRINGZ", 8):
                        return std::make_shared<Stringz>(word, copy, file_name, line_number);
#ifdef INCLUDE_ADDONS
                        case hash(".NEG", 4):
                        return std::make_shared<Neg>(word, copy, file_name, line_number);
                case hash(".SUB", 4):
                        return std::make_shared<Sub>(word, copy, file_name, line_number);
                case hash(".SET", 4):
                        return std::make_shared<Set>(word, copy, file_name, line_number);
                case hash(".LSHIFT", 7):
                        return std::make_shared<Lshift>(word, copy, file_name, line_number);
                case hash(".INCLUDE", 8):
                        return std::make_shared<Include>(word, copy, file_name, line_number);
#endif
                default:
                        break;
                }
        }

        static const std::regex decimal("#?-?\\d+", std::regex_constants::optimize);
        static const std::regex binary("-?0?[bB][01]+", std::regex_constants::optimize);
        static const std::regex hexadecimal("0?[xX][\\da-fA-F]+", std::regex_constants::optimize);
        static const std::regex octal("\\\\[0-7]+", std::regex_constants::optimize);
        static const std::regex _register("[rR]\\d", std::regex_constants::optimize);
        static const std::regex label("\\.?[\\da-zA-Z_]+", std::regex_constants::optimize);

        if (std::regex_match(word, _register)) {
                return std::make_shared<Register>(word, copy, file_name, line_number);
        } else if (std::regex_match(word, decimal)) {
                return std::make_shared<Decimal>(word, file_name, line_number);
        } else if (std::regex_match(word, binary)) {
                return std::make_shared<Binary>(word, copy, file_name, line_number);
        } else if (std::regex_match(word, hexadecimal)) {
                return std::make_shared<Hexadecimal>(word, copy, file_name, line_number);
        } else if (std::regex_match(word, octal)) {
                return std::make_shared<Octal>(word, file_name, line_number);
        } else if (std::regex_match(word, label)) {
                return std::make_shared<Label>(word, file_name, line_number);
        } else {
                const std::shared_ptr<Token> token = std::make_shared<Token>(word, copy, file_name, line_number);
                token->is_valid = false;
                return token;
        }
}

/**
 * Add a token to the current line's tokens.
 *
 * @param token The string containing the token.
 * @param to The current tokens in the line.
 * @param line_number The line number (only relevant for working with files).
 */
void Lexer::addToken(std::string &token, std::vector<std::shared_ptr<Token>> &to, int line_number)
{
        if (!token.empty()) {
                to.push_back(tokenize(token, line_number));
                token.erase();
        }
}

/**
* Go through each string (terminated by ',', space character (as defined by std::isspace),
* or a comment (denoted by ';', "//"), and tokenize it, adding it to a vector until the end
* of the line is reached.
* The end of a line is determined when either a comment is hit, or the end of the line is
* reached (that is, when the current index into the string is >= to the length of the string).
*
* @param line The line to tokenize
* @param line_number The current line number. This is only relevant when dealing with files,
*                    so defaults to 0.
*
* @return A std::vector<Token> which contains the tokens that were found in the line.
*/
void Lexer::tokenizeLine(const std::string &line, int line_number, std::vector<std::shared_ptr<Token>> &into)
{
        std::string current;

        char terminated_by { 0 };

        for (std::size_t index = 0; index < line.length();) {
                char character { line.at(index) };

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        // However, it does mean we want to check what we just got.
                        addToken(current, into, line_number);
                        terminated_by = 0;
                }

                if (character == ';') {
                        // We've hit a comment, so skip to the end of the line.
                        break;
                } else if (character == '\r') {
                        // getline doesn't consume '\r' (at least on OSX)
                        break;
                } else if (character == '/') {
                        // '//' is a comment as well.
                        if (index + 1 >= line.length() || line.at(index + 1) != '/') {
                                // It seems easiest to treat it as a comment anyways, as '/' can't be used for anything.
                                Diagnostics::Diagnostic diag(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        file_name,
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                ),
                                                Diagnostics::Variant<std::size_t>(
                                                        static_cast<std::size_t>(line_number),
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                ),
                                                Diagnostics::Variant<std::size_t>(
                                                        index,
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                )
                                        ),
                                        "Found unexpected character '/'",
                                        Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                                        Diagnostics::DIAGNOSTIC::WARNING
                                );

                                diag.provide_context(std::make_unique<Diagnostics::SelectionContext>(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        file_name,
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                ),
                                                Diagnostics::Variant<std::size_t>(
                                                        static_cast<std::size_t>(line_number),
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                ),
                                                Diagnostics::Variant<std::size_t>(
                                                        index,
                                                        Console::FOREGROUND_COLOUR::YELLOW
                                                )
                                        ),
                                        '^',
                                        "Did you mean '//'? (Treating as a comment anyways)",
                                        line,
                                        "//"
                                                     )
                                );

                                Diagnostics::push(diag);
                        }
                        break;
                } else if (character == ',') {
                        if (!into.size() || terminated_by) {
                                // TODO: Change this to a Diagnostics::push call
                                /*Logging::logger->LOG(Logging::WARNING,
                                                     line_number,
                                                     file,
                                                     "Extraneous comma.",
                                                     Logging::WARNING_TYPE::SYNTAX);*/
                        }
                        addToken(current, into, line_number);
                        terminated_by = ',';
                } else if (character == ':') {
                        if (into.size() > 1 || !current.size() || terminated_by) {
                                // TODO: Change this to a Diagnostics::push call
                                /*Logging::logger->LOG(Logging::WARNING,
                                                     line_number,
                                                     file,
                                                     "Extraneous colon.",
                                                     Logging::WARNING_TYPE::SYNTAX);*/
                        }
                        addToken(current, into, line_number);
                        terminated_by = ':';
#ifdef INCLUDE_ADDONS
                        } else if (character == '"' || character == '\'') {
#else
                } else if (character == '"') {
#endif
                        addToken(current, into, line_number);

                        char terminator { character };
                        while (index + 1 < line.length()) {
                                character = line.at(++index);
                                if (character == '\\' && line.length() > index + 1 &&
                                    line.at(index + 1) == terminator) {
                                        character = line.at(++index);
                                } else if (character == terminator) {
                                        break;
                                }
                                current += character;
                        }

                        if (character != terminator) {
                                std::stringstream stream;
#ifdef INCLUDE_ADDONS
                                stream << "Unterminated " << (terminator == '\'' ? "character" : "string");
#else
                                stream << "Unterminated string";
#endif
                                into.clear();
#ifdef INCLUDE_ADDONS
                                } else if (terminator == '\'') {
                        into.push_back(std::make_shared<Character>(current, file_name, line_number));
#endif
                        } else {
                                into.push_back(std::make_shared<String>(current, file_name, line_number));
                        }

                        current.erase();
                } else {
                        current += character;
                }
                ++index;
        }

        const bool any_valid_characters = std::any_of(
                current.cbegin(), current.cend(), [](unsigned char chr) -> bool
                {
                        return !(std::isspace(chr) || chr == ',' || chr == ':');
                }
        );

        if (any_valid_characters) {
                addToken(current, into, line_number);
        }
}
