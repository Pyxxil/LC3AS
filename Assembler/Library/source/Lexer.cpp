#include "Lexer.hpp"

#include <fstream>
#include <regex>
#include <sstream>

#include "Assembler.hpp"
#include "Tokens/Tokens.hpp"

static constexpr std::size_t hashed_letters[26] = {
        100363, 99989, 97711, 97151, 92311, 80147, 82279, 72997, 66457, 65719, 70957, 50262, 48407, 51151, 41047, 39371
        , 35401, 37039, 28697, 27791, 20201, 21523, 6449, 4813, 16333, 13337,
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
        std::size_t _hash = 37;

        for (std::size_t index = 0; index < length; ++index) {
                _hash = (_hash * hashed_letters[(static_cast<std::size_t>(string[index]) - 0x41u) % 26]) ^
                        (static_cast<std::size_t>(*string) *
                         hashed_letters[(static_cast<std::size_t>(string[index]) - 0x41u) % 26]);
        }

        return _hash;
}

static std::size_t hash(std::string &string)
{
        // Basically, we don't really want something that's likely to be a number, or a label
        if (string.at(0) != '.' && !std::isprint(string.at(0))) {
                return 0;
        }

        std::size_t _hash = 37;

        for (const auto &character : string) {
                _hash = (_hash * hashed_letters[(static_cast<std::size_t>(character) - 0x41u) % 26]) ^
                        (static_cast<std::size_t>(string.at(0)) *
                         hashed_letters[(static_cast<std::size_t>(character) - 0x41u) % 26]);
        }

        return _hash;
}

Lexer::Lexer(const std::string &t_file_name)
        : m_file_name(t_file_name), m_logger()
{

}

std::size_t Lexer::parse_into(std::vector<std::vector<std::shared_ptr<Token>>> &into)
{
        populate_tokens(into);
        return m_error_count;
}

void Lexer::populate_tokens(std::vector<std::vector<std::shared_ptr<Token>>> &into)
{
        std::ifstream file(m_file_name);

        if (file.fail()) {
                perror(m_file_name.c_str());
                exit(EXIT_FAILURE);
        }

        std::string line;

        std::vector<std::shared_ptr<Token>> tokenized_line;

        for (int line_number = 1; std::getline(file, line); line_number++) {
                if (line.empty()) {
                        continue;
                }

                tokenizeLine(line, line_number, tokenized_line);

                if (!tokenized_line.empty()) {
                        into.emplace_back(tokenized_line);
                        tokenized_line.clear();
                }
        }
}

/**
 * Using a hashed string, compare this to pre-computed (constexpr) hashes to determine what Token
 * the string represents.
 *
 * If the string begins with '#' or '-', then it is determined to be a Decimal Token. Likewise if
 * it begins with 'x' or 'X', it is a Hexadecimal number, and 'b' or 'B' it is a Binary number. If
 * it begins with '0', it could be any of the above, and this is determined by the second character
 * in the string, or is a simple Decimal 0 if it's the only character in the string.
 *
 * A string beginning with 'R' or 'r' is quite possibly a Register.
 *
 * @param word The string to tokenize
 * @param line_number The current line number. This is only relevant when assembling a file.
 * @return The Token that the string corresponds to.
 */
std::shared_ptr<Token> Lexer::tokenize(std::string &word, int line_number)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        // While this makes it a bit more efficient, is it worth double checking that
        // the strings are the same after comparing the hash's? Just as a precautionary
        // measure.
        std::size_t hashed = hash(copy);

        if (hashed) {
                switch (hashed) {
                case hash("ADD", 3):
                        return std::make_shared<Add>(word, copy, line_number);
                case hash("AND", 3):
                        return std::make_shared<And>(word, copy, line_number);
                case hash("NOT", 3):
                        return std::make_shared<Not>(word, copy, line_number);
                case hash("JSR", 3):
                        return std::make_shared<Jsr>(word, copy, line_number);
                case hash("JSRR", 4):
                        return std::make_shared<Jsrr>(word, copy, line_number);
                case hash("JMP", 3):
                        return std::make_shared<Jmp>(word, copy, line_number);
                case hash("RET", 3):
                        return std::make_shared<Ret>(word, copy, line_number);
                case hash("ST", 2):
                        return std::make_shared<St>(word, copy, line_number);
                case hash("STR", 3):
                        return std::make_shared<Str>(word, copy, line_number);
                case hash("STI", 3):
                        return std::make_shared<Sti>(word, copy, line_number);
                case hash("LD", 2):
                        return std::make_shared<Ld>(word, copy, line_number);
                case hash("LEA", 3):
                        return std::make_shared<Lea>(word, copy, line_number);
                case hash("LDI", 3):
                        return std::make_shared<Ldi>(word, copy, line_number);
                case hash("LDR", 3):
                        return std::make_shared<Ldr>(word, copy, line_number);
                case hash("PUTS", 4):
                        return std::make_shared<Puts>(word, copy, line_number);
                case hash("PUTSP", 5):
                        return std::make_shared<Putsp>(word, copy, line_number);
                case hash("HALT", 4):
                        return std::make_shared<Halt>(word, copy, line_number);
                case hash("TRAP", 4):
                        return std::make_shared<Trap>(word, copy, line_number);
                case hash("GETC", 4):
                        return std::make_shared<Getc>(word, copy, line_number);
                case hash("OUT", 3):
                        return std::make_shared<Out>(word, copy, line_number);
                case hash("IN", 2):
                        return std::make_shared<In>(word, copy, line_number);
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
                        return std::make_shared<Br>(word, copy, line_number, true, true, true);
                case hash("BRN", 3):
                        return std::make_shared<Br>(word, copy, line_number, true, false, false);
                case hash("BRZ", 3):
                        return std::make_shared<Br>(word, copy, line_number, false, true, false);
                case hash("BRP", 3):
                        return std::make_shared<Br>(word, copy, line_number, false, false, true);
                case hash("BRNZ", 4):
                        // FALLTHROUGH
                case hash("BRZN", 4):
                        return std::make_shared<Br>(word, copy, line_number, true, true, false);
                case hash("BRNP", 4):
                        // FALLTHROUGH
                case hash("BRPN", 4):
                        return std::make_shared<Br>(word, copy, line_number, true, false, true);
                case hash("BRZP", 4):
                        // FALLTHROUGH
                case hash("BRPZ", 4):
                        return std::make_shared<Br>(word, copy, line_number, false, true, true);
                case hash(".ORIG", 5):
                        return std::make_shared<Orig>(word, copy, line_number);
                case hash(".END", 4):
                        return std::make_shared<End>(word, copy, line_number);
                case hash(".FILL", 5):
                        return std::make_shared<Fill>(word, copy, line_number);
                case hash(".BLKW", 5):
                        return std::make_shared<Blkw>(word, copy, line_number);
                case hash(".STRINGZ", 8):
                        return std::make_shared<Stringz>(word, copy, line_number);
#ifdef INCLUDE_ADDONS
                case hash(".NEG", 4):
                        return std::make_shared<Neg>(word, copy, line_number);
                case hash(".SUB", 4):
                        return std::make_shared<Sub>(word, copy, line_number);
                case hash(".SET", 4):
                        return std::make_shared<Set>(word, copy, line_number);
                case hash(".LSHIFT", 7):
                        return std::make_shared<Lshift>(word, copy, line_number);
#endif
                default:
                        break;
                }
        }

        static const std::regex decimal("#?-?\\d+");
        static const std::regex binary("-?0?[bB][0-1]+");
        static const std::regex hexadecimal("0?[xX][0-9a-fA-F]+");
        static const std::regex octal("\\\\[0-7]+");
        static const std::regex _register("[rR]\\d");
        static const std::regex label("[a-zA-Z0-9_]+");

        if (std::regex_match(word, decimal)) {
                return std::make_shared<Decimal>(word, line_number);
        } else if (std::regex_match(word, binary)) {
                return std::make_shared<Binary>(word, copy, line_number);
        } else if (std::regex_match(word, hexadecimal)) {
                return std::make_shared<Hexadecimal>(word, copy, line_number);
        } else if (std::regex_match(word, octal)) {
                return std::make_shared<Octal>(word, line_number);
        } else if (std::regex_match(word, _register)) {
                return std::make_shared<Register>(word, copy, line_number);
        } else if (std::regex_match(word, label)) {
                return std::make_shared<Label>(word, line_number);
        } else {
                const std::shared_ptr<Token> token = std::make_shared<Token>(word, copy, line_number);
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

        char terminated_by = 0;

        for (std::size_t index = 0; index < line.length();) {
                char character = line.at(index);

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
                        if (index + 1 > line.length() || line.at(index + 1) != '/') {
                                // It seems easiest to treat it as a comment anyways, as '/' can't be used for anything.
                                m_logger.LOG(Logger::WARNING, line_number, "Expected '//', but found '/'. "
                                        "Treating it as if it's '//' (i.e. comment).", Logger::SYNTAX);
                        }
                        break;
                } else if (character == ',') {
                        if (!into.size() || terminated_by) {
                                m_logger.LOG(Logger::WARNING, line_number, "Extraneous comma.", Logger::SYNTAX);
                        }
                        addToken(current, into, line_number);
                        terminated_by = ',';
                } else if (character == ':') {
                        if (into.size() > 1 || !current.size() || terminated_by) {
                                m_logger.LOG(Logger::WARNING, line_number, "Extraneous colon.", Logger::SYNTAX);
                        }
                        addToken(current, into, line_number);
                        terminated_by = ':';
#ifdef INCLUDE_ADDONS
                } else if (character == '"' || character == '\'') {
#else
                        } else if (character == '"') {
#endif
                        addToken(current, into, line_number);

                        char terminator = character;
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
                                m_logger.LOG(Logger::ERROR, line_number, stream.str());
                                ++m_error_count;
                                into.clear();
#ifdef INCLUDE_ADDONS
                        } else if (terminator == '\'') {
                                into.push_back(std::make_shared<Character>(current, line_number));
#endif
                        } else {
                                into.push_back(std::make_shared<String>(current, line_number));
                        }

                        current.erase();
                } else {
                        current += character;
                }
                ++index;
        }

        const bool any_valid_characters = std::any_of(current.cbegin(), current.cend(), [](unsigned char chr) -> bool
        {
                return !(std::isspace(chr) || chr == ',' || chr == ':');
        });

        if (any_valid_characters) {
                addToken(current, into, line_number);
        }
}
