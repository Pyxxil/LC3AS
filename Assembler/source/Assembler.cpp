#include "Assembler.hpp"

#include "Tokens/All_Tokens.hpp"

#include <fstream>

static constexpr std::size_t hashed_letters[26] = {
        100363, 99989, 97711, 97151, 92311, 80147,
        82279, 72997, 66457, 65719, 70957, 50262,
        48407, 51151, 41047, 39371, 35401, 37039,
        28697, 27791, 20201, 21523, 6449, 4813,
        16333, 13337,
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
        std::size_t first_char_on_directive = length > 1 ? static_cast<std::size_t>(*(string + 1)) : 0;

        for (std::size_t index = 0; index < length; ++index) {
                if (*string == '.') {
                        if (*(string + index) == '.') {
                                _hash = (_hash * hashed_letters[first_char_on_directive - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[first_char_on_directive - 0x41u]);
                        } else {
                                _hash = (_hash * hashed_letters[static_cast<std::size_t>(*(string + index)) - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[
                                                static_cast<std::size_t>(*(string + index)) - 0x41u]);
                        }
                } else {
                        _hash = (_hash * hashed_letters[static_cast<std::size_t>(*(string + index)) - 0x41u]) ^
                                (static_cast<std::size_t>(*string) *
                                        hashed_letters[static_cast<std::size_t>(*(string + index)) - 0x41u]);
                }
        }

        return _hash;
}

static std::size_t hash(std::string &string)
{
        if (string.at(0) != '.' && !std::isalpha(string.at(0))) {
                return 0;
        }

        std::size_t _hash = 37;
        std::size_t first_char_on_directive = string.length() > 1 ? static_cast<std::size_t>(string.at(1)) : 0;

        for (const auto &character : string) {
                if (string.at(0) == '.') {
                        if (character == '.') {
                                _hash = (_hash * hashed_letters[first_char_on_directive - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[first_char_on_directive - 0x41u]);
                        } else {
                                _hash = (_hash * hashed_letters[static_cast<std::size_t>(character) - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[
                                                static_cast<std::size_t>(character) - 0x41u]);
                        }
                } else {
                        _hash = (_hash * hashed_letters[static_cast<std::size_t>(character) - 0x41u]) ^
                                (static_cast<std::size_t>(string.at(0)) *
                                        hashed_letters[static_cast<std::size_t>(character) - 0x41u]);
                }
        }

        return _hash;
}

Assembler::Assembler()
        : error_count(0)
          , internal_program_counter(0)
          , origin_seen(false)
          , end_seen(false)
{
}

/**
 * Given a file name, open the file, then tokenize it line by line.
 *
 * @param fileName The name of the file to read.
 * @return A 2D std::vector containing each tokenized line.
 */
std::vector<std::vector<std::shared_ptr<Token>>> &Assembler::tokenizeFile(std::string &fileName)
{
        error_count = 0;  // TODO: Is this the right choice? What if we want to keep them?

        std::ifstream file(fileName);

        if (!file.is_open()) {
                perror(PROJECT);
                exit(EXIT_FAILURE);
        }

        std::string line;
        int line_number = 0;

        while (std::getline(file, line)) {
                line_number++;

                if (line.empty()) {
                        continue;
                }

                std::vector<std::shared_ptr<Token>> tokenized_line = tokenizeLine(line, line_number);
                if (!tokenized_line.empty()) {
                        tokens.push_back(tokenized_line);
                }

                if (!tokens.empty() && !tokens.back().empty()) {
                        error_count += tokens.back().front()->is_error;
                }
        }

        assemble();
        assembled();
        return tokens;
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
std::vector<std::shared_ptr<Token>> Assembler::tokenizeLine(std::string &line, int line_number)
{
        char character;

        std::string current;

        std::vector<std::shared_ptr<Token>> tokenenized_line;

        for (std::size_t index = 0; index < line.length();) {
                character = line.at(index);

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        // However, it does mean we want to check what we just got.
                        addToken(current, tokenenized_line, line_number);
                }

                if (character == ';') {
                        // We've hit a comment, so skip to the end of the line.
                        break;
                } else if (character == '/') {
                        // '//' is a comment as well.
                        if (index + 1 > line.length() || line.at(index + 1) != '/') {
                                // It seems easiest to treat it as a comment anyways, as '/' can't be used for anything.
                                WARNING("Expected '//', but found '/'. Treating it as if it's '//' (i.e. comment)");
                        }
                        break;
                } else if (character == ',' || character == ':') {
                        addToken(current, tokenenized_line, line_number);
                } else if (character == '"') {
                        addToken(current, tokenenized_line, line_number);

                        char last_character = 0;
                        while (index + 1 < line.length()) {
                                last_character = line.at(++index);
                                if (last_character == '"') {
                                        break;
                                }
                                current += last_character;
                        }

                        if (index == line.length() && last_character != '"') {
                                tokenenized_line.push_back(std::make_shared<String>(current, line_number));
                                tokenenized_line.back()->expected("to find closing '\"'");
                        } else {
                                tokenenized_line.push_back(std::make_shared<String>(current, line_number));
                        }

                        current.erase();
                } else {
                        current += character;
                }
                ++index;
        }

        addToken(current, tokenenized_line, line_number);

        return tokenenized_line;
}

/**
 * Add a token to the current line's tokens.
 *
 * @param token The string containing the token.
 * @param toks The current tokens in the line.
 * @param line_number The line number (only relevant for working with files).
 */
void Assembler::addToken(std::string &token, std::vector<std::shared_ptr<Token>> &toks, int line_number)
{
        if (!token.empty()) {
                toks.push_back(tokenize(token, line_number));
                token.erase();
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
std::shared_ptr<Token> Assembler::tokenize(std::string &word, int line_number)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        // While this makes it a bit more efficient, is it worth double checking that
        // the strings are the same after comparing the hash's? Just as a precautionary
        // measure.
        std::size_t hashed = hash(copy);

        switch (copy.at(0)) {
        case '0':
                if (copy.length() == 1) {
                        return std::make_shared<Decimal>(word, line_number);
                } else if (copy.at(1) == 'X') {
                        return std::make_shared<Hexadecimal>(word, line_number);
                } else if (copy.at(1) == 'B') {
                        return std::make_shared<Binary>(word, line_number);
                } else {
                        return std::make_shared<Decimal>(word, line_number);
                }
        case '#':  // FALLTHROUGH
        case '-':
                return std::make_shared<Decimal>(copy, line_number);
        case 'B':
                if (copy.length() > 1 && (copy.at(1) == '0' || copy.at(1) == '1')) {
                        copy = word.substr(1);
                        return std::make_shared<Binary>(copy, line_number);
                }
                break;
        case 'X':
                if (copy.length() > 1 && std::isxdigit(copy.at(1))) {
                        return std::make_shared<Hexadecimal>(word, line_number);
                }
                return std::make_shared<Label>(word, line_number);
        case 'R':
                if (copy.length() < 2) {
                        return std::make_shared<Label>(word, line_number);
                }
                if (std::isdigit(copy.at(1))) {
                        if (copy.length() < 3 || std::isspace(copy.at(2)) || copy.at(2) == ',') {
                                return std::make_shared<Register>(word);
                        }
                } else if (hashed == hash("RET", 3)) {
                        return std::make_shared<Ret>(word, line_number);
                }
                return std::make_shared<Label>(word, line_number);
        default:
                break;
        }

        switch (hashed) {
        case hash("ADD", 3):
                return std::make_shared<Add>(word, line_number);
        case hash("AND", 3):
                return std::make_shared<And>(word, line_number);
        case hash("NOT", 3):
                return std::make_shared<Not>(word, line_number);
        case hash("JSR", 3):
                return std::make_shared<Jsr>(word, line_number);
        case hash("JSRR", 4):
                return std::make_shared<Jsrr>(word, line_number);
        case hash("JMP", 3):
                return std::make_shared<Jmp>(word, line_number);
        case hash("ST", 2):
                return std::make_shared<St>(word, line_number);
        case hash("STR", 3):
                return std::make_shared<Str>(word, line_number);
        case hash("STI", 3):
                return std::make_shared<Sti>(word, line_number);
        case hash("LD", 2):
                return std::make_shared<Ld>(word, line_number);
        case hash("LEA", 3):
                return std::make_shared<Lea>(word, line_number);
        case hash("LDI", 3):
                return std::make_shared<Ldi>(word, line_number);
        case hash("LDR", 3):
                return std::make_shared<Ldr>(word, line_number);
        case hash("PUTS", 4):
                return std::make_shared<Puts>(word, line_number);
        case hash("PUTSP", 5):
                return std::make_shared<Putsp>(word, line_number);
        case hash("HALT", 4):
                return std::make_shared<Halt>(word, line_number);
        case hash("TRAP", 4):
                return std::make_shared<Trap>(word, line_number);
        case hash("GETC", 4):
                return std::make_shared<Getc>(word, line_number);
        case hash("OUT", 3):
                return std::make_shared<Out>(word, line_number);
        case hash("IN", 2):
                return std::make_shared<In>(word, line_number);
        case hash("BR", 2):
                // FALLTHROUGH
        case hash("BRNZP", 5):
        case hash("BRNPZ", 5):
                // FALLTHROUGH
        case hash("BRZNP", 5):
        case hash("BRZPN", 5):
                // FALLTHROUGH
        case hash("BRPNZ", 5):
        case hash("BRPZN", 5):
                return std::make_shared<Br>(word, line_number, true, true, true);
        case hash("BRN", 3):
                return std::make_shared<Br>(word, line_number, true, false, false);
        case hash("BRZ", 3):
                return std::make_shared<Br>(word, line_number, false, true, false);
        case hash("BRP", 3):
                return std::make_shared<Br>(word, line_number, false, false, true);
        case hash("BRNZ", 4):
        case hash("BRZN", 4):
                return std::make_shared<Br>(word, line_number, true, true, false);
        case hash("BRNP", 4):
        case hash("BRPN", 4):
                return std::make_shared<Br>(word, line_number, true, false, true);
        case hash("BRZP", 4):
        case hash("BRPZ", 4):
                return std::make_shared<Br>(word, line_number, false, true, true);
        case hash(".ORIG", 5):
                return std::make_shared<Orig>(word, line_number);
        case hash(".END", 4):
                return std::make_shared<End>(word, line_number);
        case hash(".FILL", 5):
                return std::make_shared<Fill>(word, line_number);
        case hash(".BLKW", 5):
                return std::make_shared<Blkw>(word, line_number);
        case hash(".STRINGZ", 8):
                return std::make_shared<Stringz>(word, line_number);
        default:
                // Of course, if it doesn't match the above, then we'll treat it as a label.
                return std::make_shared<Label>(word, line_number);
        }
}

void Assembler::assemble()
{
        if (tokens.empty()) {
                return;
        }

        std::int32_t memory_required = 0;

        // It would be best to go through the tokens line by line and check the first element
        // and if it's a LABEL, add it to the symbol table, otherwise don't worry about it.
        std::cout << "Starting first pass\n";
        for (auto &tokenized_line : tokens) {
                switch (tokenized_line.front()->type()) {
                case Token::DIR_ORIG:
                        memory_required = tokenized_line.front()->assemble(tokenized_line, *this);
                        if (memory_required < 0) {
                                ++error_count;
                        } else {
                                internal_program_counter = file_memory_origin_address =
                                        static_cast<std::uint16_t>(memory_required);
                        }
                        break;
                case Token::LABEL:
                        std::static_pointer_cast<Label>(tokenized_line.front())->address = internal_program_counter;
                        if (symbols.count(internal_program_counter)) {
                                WARNING("Multiple labels found at address %d", file_memory_origin_address);
                                WARNING("\tPrevious label '%s' found on line %d", tokenized_line.front()->word.c_str(),
                                        tokenized_line.front()->at_line);
                        }
                        symbols.insert(std::pair<std::uint16_t, std::shared_ptr<Label>>(
                                file_memory_origin_address,
                                std::static_pointer_cast<Label>(tokenized_line.front()))
                        );
                case Token::DIR_END:            // FALLTHROUGH
                case Token::DIR_BLKW:           // FALLTHROUGH
                case Token::DIR_FILL:           // FALLTHROUGH
                case Token::DIR_STRINGZ:        // FALLTHROUGH
                        memory_required = tokenized_line.front()->assemble(tokenized_line, *this);
                        if (memory_required < 0) {
                                ++error_count;
                        } else {
                                internal_program_counter += memory_required;
                        }
                        break;
                default:
                        if (!origin_seen) {
                                tokenized_line.front()->expected(".ORIG statement");
                                ++error_count;
                                continue;
                        }

                        if (end_seen) {
                                WARNING("%s found after .END. It will be ignored",
                                        tokenized_line.front()->word.c_str());
                        } else {
                                ++internal_program_counter;
                        }
                }
        }

        std::cout << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the first pass\n";

        if (error_count) {
                return;
        }

        end_seen = false;
        origin_seen = false;
        internal_program_counter = file_memory_origin_address;

        std::cout << "Starting second pass\n";

        for (auto &tokenized_line : tokens) {
                // This should return >= 0 on success (where the value is then used to advance the pc),
                // -1 if there was an error.
                // Arguably, it's probably better to pass a reference to the assembler class with this, and
                // from that it can be determined if the origin has been seen, or if the end has been seen,
                // and if a label is actually in the file.
                memory_required = tokenized_line.front()->assemble(tokenized_line, *this);

                if (memory_required == -1) {
                        error_count++;
                } else if (memory_required > 0) {
                        internal_program_counter += memory_required;
                }
        }

        std::cout << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the second pass\n";
}

std::vector<std::uint16_t> Assembler::assembled()
{
        std::vector<std::uint16_t> assembled_tokens;

        for (const auto &tokenized_line : tokens) {
                for (const auto &assembled_line : tokenized_line.front()->as_assembled()) {
                        assembled_tokens.push_back(assembled_line);
                }
        }

        return assembled_tokens;
}
