#include "../includes/Assembler.hpp"
#include "../includes/Token_String.hpp"
#include "../includes/Token_Directive_Orig.hpp"
#include "../includes/Token_Directive_Fill.hpp"
#include "../includes/Token_Directive_Blkw.hpp"
#include "../includes/Token_Directive_End.hpp"
#include "../includes/Token_Immediate_Hexadecimal.hpp"
#include "../includes/Token_Label.hpp"
#include "../includes/Token_Immediate_Binary.hpp"
#include "../includes/Token_Directive_Stringz.hpp"
#include "../includes/Token_Immediate_Decimal.hpp"
#include "../includes/Token_Register.hpp"
#include "../includes/Token_Operator_Add.hpp"
#include "../includes/Token_Operator_Br.hpp"
#include "../includes/Token_Trap_In.hpp"
#include "../includes/Token_Trap_Getc.hpp"
#include "../includes/Token_Trap_Out.hpp"
#include "../includes/Token_Operator_Trap.hpp"
#include "../includes/Token_Trap_Halt.hpp"
#include "../includes/Token_Trap_Putsp.hpp"
#include "../includes/Token_Trap_Puts.hpp"
#include "../includes/Token_Operator_Ldr.hpp"
#include "../includes/Token_Operator_Ldi.hpp"
#include "../includes/Token_Operator_Lea.hpp"
#include "../includes/Token_Operator_Ld.hpp"
#include "../includes/Token_Operator_Sti.hpp"
#include "../includes/Token_Operator_Str.hpp"
#include "../includes/Token_Operator_St.hpp"
#include "../includes/Token_Operator_Jmp.hpp"
#include "../includes/Token_Operator_Jsrr.hpp"
#include "../includes/Token_Operator_Jsr.hpp"
#include "../includes/Token_Operator_Not.hpp"
#include "../includes/Token_Operator_And.hpp"
#include "../includes/Token_Operator_Ret.hpp"

#include <fstream>

static constexpr std::size_t hashed_letters[26] = {
        100363, 99989, 97711, 97151, 92311, 80147,
        82279,  72997, 66457, 65719, 70957, 50262,
        48407,  51151, 41047, 39371, 35401, 37039,
        28697,  27791, 20201, 21523, 6449,  4813,
        16333,  13337,
};

static constexpr std::size_t hash(const char *const string, std::size_t length) {
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
        : errors(0), file_memory_orig_addr(0)
{
        // TODO: Instead of the pre-computed hash's, maybe compute them here?
        // TODO:        - Possibly by having constexpr std::size_t hash(const char *string)
        // TODO: Of course, this would require more memory.
        // TODO: But what about as class members?
}

/**
 * Given a file name, open the file, then tokenize it line by line.
 *
 * @param fileName The name of the file to read.
 * @return A 2D std::vector containing each tokenized line.
 */
std::vector<std::vector<std::unique_ptr<Token>>> Assembler::tokenizeFile(std::string &fileName)
{
        errors = 0;  // TODO: Is this the right choice? What if we want to keep them?

        std::ifstream file(fileName);

        if (!file.is_open()) {
                perror(PROJECT);
                exit(EXIT_FAILURE);
        }

        std::string line;
        int line_number = 0;

        std::vector<std::vector<std::unique_ptr<Token>>> tokens;

        while (std::getline(file, line)) {
                line_number++;

                if (line.empty()) {
                        continue;
                }

                std::vector<std::unique_ptr<Token>> tokenized_line = tokenizeLine(line, line_number);
                if (!tokenized_line.empty()) {
                        tokens.push_back(std::move(tokenized_line));
                }

                if (!tokens.empty() && !tokens.back().empty()) {
                        errors += tokens.back().front()->is_error;
                }

                std::cout << "\n";
        }

        generate_symbols(tokens);

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
std::vector<std::unique_ptr<Token>> Assembler::tokenizeLine(std::string &line, int line_number)
{
        char character;

        std::string current;

        std::cout << "Going in: " << line << "\n";

        std::vector<std::unique_ptr<Token>> tokens;

        for (std::size_t index = 0; index < line.length(); ) {
                character = line.at(index);

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        // However, it does mean we want to check what we just got.
                        addToken(current, tokens, line_number);
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
                        addToken(current, tokens, line_number);
                } else if (character == '"')  {
                        addToken(current, tokens, line_number);

                        char last_character = 0;
                        while (index + 1 < line.length()) {
                                last_character = line.at(++index);
                                if (last_character == '"') {
                                        break;
                                }
                                current += last_character;
                        }

                        if (index == line.length() && last_character != '"') {
                                tokens.push_back(std::unique_ptr<Token>{ new String(current, line_number) });
                                tokens.back()->expected("to find closing '\"'");
                        } else {
                                tokens.push_back(std::unique_ptr<Token> { new String(current, line_number ) });
                        }

                        current.erase();
                } else {
                        current += character;
                }
                ++index;
        }

        addToken(current, tokens, line_number);

        return tokens;
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
std::unique_ptr<Token> Assembler::tokenize(std::string &word, int line_number)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        // While this makes it a bit more efficient, is it worth double checking?
        std::size_t hashed = hash(copy);

        switch (copy.at(0)) {
        case '0':
                if (copy.length() == 1) {
                        return std::unique_ptr<Token>{ new Decimal(word, line_number) };
                } else if (copy.at(1) == 'X') {
                        return std::unique_ptr<Token>{ new Hexadecimal(word, line_number) };
                } else if (copy.at(1) == 'B') {
                        return std::unique_ptr<Token>{ new Binary(word, line_number) };
                } else {
                        return std::unique_ptr<Token>{ new Decimal(word, line_number) };
                }
        case '#':  // FALLTHROUGH
        case '-':
                return std::unique_ptr<Token>{ new Decimal(copy, line_number) };
        case 'B':
                if (copy.length() > 1 && (copy.at(1) == '0' || copy.at(1) == '1')) {
                        copy = word.substr(1);
                        return std::unique_ptr<Token>{ new Binary(copy, line_number) };
                }
                break;
        case 'X':
                if (copy.length() > 1 && std::isxdigit(copy.at(1))) {
                        return std::unique_ptr<Token>{ new Hexadecimal(word, line_number) };
                }
                return std::unique_ptr<Token>{ new Label(word, line_number) };
        case 'R':
                if (copy.length() < 2) {
                        return std::unique_ptr<Token>{ new Label(word, line_number) };
                }
                if (std::isdigit(copy.at(1))) {
                        if (copy.length() < 3 || std::isspace(copy.at(2)) || copy.at(2) == ',') {
                                return std::unique_ptr<Token>{ new Register(word) };
                        }
                } else if (hashed == hash("RET", 3)) {
                        return std::unique_ptr<Token>{ new Ret(word, line_number) };
                }
                return std::unique_ptr<Token>{ new Label(word, line_number) };
        default:
                break;
        }

        switch (hashed) {
        case hash("ADD", 3):
                return std::unique_ptr<Token>{ new Add(word, line_number) };
        case hash("AND", 3):
                return std::unique_ptr<Token>{ new And(word, line_number) };
        case hash("NOT", 3):
                return std::unique_ptr<Token>{ new Not(word, line_number) };
        case hash("JSR", 3):
                return std::unique_ptr<Token>{ new Jsr(word, line_number) };
        case hash("JSRR", 4):
                return std::unique_ptr<Token>{ new Jsrr(word, line_number) };
        case hash("JMP", 3):
                return std::unique_ptr<Token>{ new Jmp(word, line_number) };
        case hash("ST", 2):
                return std::unique_ptr<Token>{ new St(word, line_number) };
        case hash("STR", 3):
                return std::unique_ptr<Token>{ new Str(word, line_number) };
        case hash("STI", 3):
                return std::unique_ptr<Token>{ new Sti(word, line_number) };
        case hash("LD", 2):
                return std::unique_ptr<Token>{ new Ld(word, line_number) };
        case hash("LEA", 3):
                return std::unique_ptr<Token>{ new Lea(word, line_number) };
        case hash("LDI", 3):
                return std::unique_ptr<Token>{ new Ldi(word, line_number) };
        case hash("LDR", 3):
                return std::unique_ptr<Token>{ new Ldr(word, line_number) };
        case hash("PUTS", 4):
                return std::unique_ptr<Token>{ new Puts(word, line_number) };
        case hash("PUTSP", 5):
                return std::unique_ptr<Token>{ new Putsp(word, line_number) };
        case hash("HALT", 4):
                return std::unique_ptr<Token>{ new Halt(word, line_number) };
        case hash("TRAP", 4):
                return std::unique_ptr<Token>{ new Trap(word, line_number) };
        case hash("GETC", 4):
                return std::unique_ptr<Token>{ new Getc(word, line_number) };
        case hash("OUT", 3):
                return std::unique_ptr<Token>{ new Out(word, line_number) };
        case hash("IN", 2):
                return std::unique_ptr<Token>{ new In(word, line_number) };
        case hash("BR", 2):
                // FALLTHROUGH
        case hash("BRNZP", 5): case hash("BRNPZ", 5):
                // FALLTHROUGH
        case hash("BRZNP", 5): case hash("BRZPN", 5):
                // FALLTHROUGH
        case hash("BRPNZ", 5): case hash("BRPZN", 5):
                return std::unique_ptr<Token>{ new Br(word, line_number, true, true, true) };
        case hash("BRN", 3):
                return std::unique_ptr<Token>{ new Br(word, line_number, true, false, false) };
        case hash("BRZ", 3):
                return std::unique_ptr<Token>{ new Br(word, line_number, false, true, false) };
        case hash("BRP", 3):
                return std::unique_ptr<Token>{ new Br(word, line_number, false, false, true) };
        case hash("BRNZ", 4): case hash("BRZN", 4):
                return std::unique_ptr<Token>{ new Br(word, line_number, true, true, false) };
        case hash("BRNP", 4): case hash("BRPN", 4):
                return std::unique_ptr<Token>{ new Br(word, line_number, true, false, true) };
        case hash("BRZP", 4): case hash("BRPZ", 4):
                return std::unique_ptr<Token>{ new Br(word, line_number, false, true, true) };
        case hash(".ORIG", 5):
                return std::unique_ptr<Token>{ new Orig(word, line_number) };
        case hash(".END", 4):
                return std::unique_ptr<Token>{ new End(word, line_number) };
        case hash(".FILL", 5):
                return std::unique_ptr<Token>{ new Fill(word, line_number) };
        case hash(".BLKW", 5):
                return std::unique_ptr<Token>{ new Blkw(word, line_number) };
        case hash(".STRINGZ", 8):
                return std::unique_ptr<Token>{ new Stringz(word, line_number) };
        default:
                break;
        }

        std::cout << "DEBUG: " << copy << " hashed to " << std::hex << hashed << "\n";
        return std::unique_ptr<Token>{ new Label(word, line_number) };
}

bool Assembler::validate(std::vector<Token> &tokens)
{
        (void) tokens;
        return false;
}

std::map<Token, std::uint16_t> &Assembler::generate_symbols(std::vector<std::vector<std::unique_ptr<Token>>> &tokens)
{
        std::size_t orig_index = 0;

        if (tokens.empty()) {
                return symbols;
        }

        for (; orig_index < tokens.size(); ++orig_index) {
               if (tokens.at(orig_index).front()->type() == Token::token_type::DIR_ORIG) {
                       if (tokens[orig_index].front()->validate(tokens[orig_index])) {
                               file_memory_orig_addr =
                                       dynamic_cast<Immediate *>(tokens[orig_index].back().get())->immediate;
                       }
                       errors++;
                       break;
               } else {
                       tokens[orig_index].front()->expected(".ORIG directive");
                       ++errors;
               }
        }

        //for (const auto & tokenizedLine : tokens) {
        //}
        return symbols;
}

void Assembler::addToken(std::string &token, std::vector<std::unique_ptr<Token>> &tokens, int line_number)
{
        if (!token.empty()) {
                tokens.push_back(tokenize(token, line_number));
                std::cout << tokens.back()->word << " ";
                token.erase();
        }
}
