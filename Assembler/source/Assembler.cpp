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

static std::size_t hash(std::string &string)
{
        static const std::size_t hashed_letters[26] = {
                100363, 99989, 97711, 97151, 92311, 80147,
                82279,  72997, 66457, 65719, 70957, 50262,
                48407,  51151, 41047, 39371, 35401, 37039,
                28697,  27791, 20201, 21523, 6449,  4813,
                16333,  13337,
        };

        if (string.at(0) != '.' && !std::isalpha(string.at(0))) {
                return 0;
        }

        std::size_t _hash = 37;
        std::size_t first_char_on_directive = static_cast<std::size_t>(string.at(1));

        for (const auto &character : string) {
                if (string.at(0) == '.') {
                        if (character == '.') {
                                _hash = (_hash * hashed_letters[first_char_on_directive - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[first_char_on_directive - 0x41u]);
                        } else {
                                _hash = (_hash * hashed_letters[static_cast<std::size_t>(character) - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[static_cast<std::size_t>(character) - 0x41u]);
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
{

}

std::vector<std::vector<Token>> Assembler::tokenizeFile(std::string &fileName)
{
        std::ifstream file(fileName);

        if (!file.is_open()) {
                perror(PROJECT);
                exit(EXIT_FAILURE);
        }

        std::string line;
        int line_number = 0;
        int errors = 0;

        std::vector<std::vector<Token>> tokens;

        while (std::getline(file, line)) {
                line_number++;

                if (line.empty()) {
                        continue;
                }

                tokens.push_back(tokenizeLine(line, line_number));
                generate_symbols(tokens.back());

                if (!tokens.empty() && !tokens.back().empty()) {
                        errors += tokens.back().front().is_error;
                }

                std::cout << std::endl;
        }

        return tokens;
}

std::vector<Token> Assembler::tokenizeLine(std::string &line, int line_number)
{
        char character;

        std::string current;

        std::cout << "Going in: " << line << std::endl;

        std::vector<Token> tokens;

        for (std::size_t index = 0; index < line.length(); /* Is this the place to update? */) {
                character = line.at(index);

                // Deal with strings here, it'll be much simpler.

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        // However, it does mean we want to check what we just got.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

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
                                tokens.push_back(String(current, line_number).expected("to find closing '\"'"));
                        } else {
                                tokens.push_back(String(current, line_number));
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

Token Assembler::tokenize(std::string &word, int line_number)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        // While this makes it a bit more efficient, is it worth double checking?
        std::size_t hashed = hash(copy);

        switch (copy.at(0)) {
        case '.':
                switch (hashed) {
                case 0xae00dbad81af1338:  // hash(".ORIG")
                        return Orig(word, line_number);
                case 0xd143ba5fa8981851:  // hash(".END")
                        return End(word, line_number);
                case 0x66e0b0b6cd5a4e20:  // hash(".FILL")
                        return Fill(word, line_number);
                case 0xb97d9f7e2d9fd702:  // hash(".BLKW")
                        return Blkw(word, line_number);
                case 0xef479fa15c8dd7c1:  // hash(".STRINGZ")
                        return Stringz(word, line_number);
                default:
                        return Token(word, line_number).expected("one of .FILL, .ORIG, .END, .STRINGZ, .BLKW");
                }
        case '0':
                if (copy.length() == 1) {
                        return Decimal(word, line_number);
                } else if (copy.at(1) == 'X') {
                        return Hexadecimal(word, line_number);
                } else if (copy.at(1) == 'B') {
                        return Binary(word, line_number);
                } else {
                        return Decimal(word, line_number);
                }
        case '#':  // FALLTHROUGH
        case '-':
                return Decimal(copy, line_number);
        case 'B':
                if (copy.length() > 1 && (copy.at(1) == '0' || copy.at(1) == '1')) {
                        copy = word.substr(1);
                        return Binary(copy, line_number);
                }
                break;
        case 'X':
                if (copy.length() > 1 && std::isxdigit(copy.at(1))) {
                        return Hexadecimal(word, line_number);
                }
                return Label(word, line_number);
        case 'R':
                if (copy.length() < 2) {
                        return Label(word, line_number);
                }
                if (std::isdigit(copy.at(1))) {
                        if (copy.length() < 3 || std::isspace(copy.at(2)) || copy.at(2) == ',') {
                                return Register(word);
                        }
                } else if (hashed == 0x230e9051f39cad) { // hash("RET")
                        return Ret(word, line_number);
                }
                return Label(word, line_number);
        default:
                break;
        }


        switch (hashed) {
        case 0x00c847e7858f3bda:  // hash("ADD")
                return Add(word, line_number);
        case 0x006972ca4e0fe6aa:  // hash("AND")
                return And(word, line_number);
        case 0x000880559a3c58a1:  // hash("NOT")
                return Not(word, line_number);
        case 0x001b83c7f078f08f:  // hash("JSR")
                return Jsr(word, line_number);
        case 0x8cef8cf169d53357:  // hash("JSRR")
                return Jsrr(word, line_number);
        case 0x003155da34ef9599:  // hash("JMP")
                return Jmp(word, line_number);
        case 0x000000163a87a587:  // hash("ST")
                return St(word, line_number);
        case 0x000c901e4ff8fff4:  // hash("STR")
                return Str(word, line_number);
        case 0x00168a8037dda834:  // hash("STI")
                return Sti(word, line_number);
        case 0x000000389286e2ae:  // hash("LD")
                return Ld(word, line_number);
        case 0x005251df343da02e:  // hash("LEA")
                return Lea(word, line_number);
        case 0x00395e0e09be9292:  // hash("LDI")
                return Ldi(word, line_number);
        case 0x001ff918099c2706:  // hash("LDR")
                return Ldr(word, line_number);
        case 0x2fd2a79caa1c2dd9:  // hash("PUTS")
                return Puts(word, line_number);
        case 0x45f5f141ac74c8d6:  // hash("HALT")
                return Halt(word, line_number);
        case 0xf8bf61331cc727e5:  // hash("TRAP")
                return Trap(word, line_number);
        case 0x41ca364764d222c1:  // hash("GETC")
                return Getc(word, line_number);
        case 0x000502dd326219b2:  // hash("OUT")
                return Out(word, line_number);
        case 0x0000005709aa5303:  // hash("IN")
                return In(word, line_number);
        case 0x000000346c5d7733:  // hash("BR")
                // FALLTHROUGH
        case 0x6c916d80285dac45:  // hash("BRNZP")
                // FALLTHROUGH
        case 0x6c916d7ab103e58d:  // hash("BRNPZ")
                // FALLTHROUGH
        case 0x6c9902f926350905:  // hash("BRZNP")
                // FALLTHROUGH
        case 0x6c990314abe22845:  // hash("BRZPN")
                // FALLTHROUGH
        case 0x6c9c36bfd651af8d:  // hash("BRPNZ")
                // FALLTHROUGH
        case 0x6c9c36c1713fab8d:  // hash("BRPZN")
                return Br(word, line_number, true, true, true);
        case 0x0028eaa0470f8463:  // hash("BRN")
                return Br(word, line_number, true, false, false);
        case 0x000aab21915b9189:  // hash("BRZ")
                return Br(word, line_number, false, true, false);
        case 0x001f7e55ca7ca627:  // hash("BRp")
                return Br(word, line_number, false, false, true);
        case 0x53a77816176567d9:  // hash("BRNZ")
                // FALLTHROUGH
        case 0x53a77822b71faf99:  // hash("BRZN")
                return Br(word, line_number, true, true, false);
        case 0x94abd7909f4a83d7:  // hash("BRNP")
                // FALLTHROUGH
        case 0x94abd7c59fc129d7:  // hash("BRPN")
                return Br(word, line_number, true, false, true);
        case 0x68bcc37dfdb0eef5:  // hash("BRZP")
                // FALLTHROUGH
        case 0x68bcc38217e14bbd:  // hash("BRPZ")
                return Br(word, line_number, false, true, true);
        default:
                break;
        }

        // DEBUG: next line
        std::cout << copy << " hashed to " << std::hex << hashed << std::endl;
        return Label(word, line_number);
}

bool Assembler::validate(std::vector<Token> &tokens)
{
        (void) tokens;
        return false;
}

std::map<Token, uint16_t> &Assembler::generate_symbols(std::vector<Token, std::allocator<Token>> &tokens)
{
        (void) tokens;
        return symbols;
}

void Assembler::addToken(std::string &token, std::vector<Token> &tokens, int line_number)
{
        if (!token.empty()) {
                tokens.push_back(tokenize(token, line_number));
                std::cout << tokens.back().word << " ";
                token.erase();
        }
}
