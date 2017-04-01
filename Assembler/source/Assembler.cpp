#include "../includes/Assembler.hpp"

#include <fstream>

Assembler::Assembler()
{

}

std::vector<std::vector<Token>> Assembler::tokenizeFile(std::string &fileName)
{
        std::ifstream file(fileName);
        //file.open(fileName);

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
        (void) line_number;
        char character;

        std::string current;

        std::cout << "Going in: " << line << std::endl;

        std::vector<Token> tokens;

        for (std::size_t index = 0; index < line.length(); /* Is this the place to update? */) {
                character = line.at(index);

                // Deal with strings here, it'll be much simpler.

                if (std::isspace(character)) {
                        //std::cout << "Found some whitespace, skipping and dumping contents" << std::endl;
                        // We don't care about space characters.
                        // However, it does mean we want to check what we just got.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        addToken(current, tokens, line_number);
                }

                if (character == ';') {
                        // We've hit a comment, so skip to the end of the line.
                        //std::cout << "Reached comment, so end of line" << std::endl;
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

        switch (copy.at(0)) {
        case '.':
                if (copy == ".ORIG") {
                        return Orig(word);
                } else if (copy == ".END") {
                        return End(word);
                } else if (copy == ".FILL") {
                        return Fill(word);
                } else if (copy == ".BLKW") {
                        return Blkw(word);
                } else if (copy == ".STRINGZ") {
                        return Stringz(word);
                }
                return Token(word, line_number).expected("one of .FILL, .ORIG, .END, .STRINGZ, .BLKW");
        case '0':
                if (copy.at(1) == 'X') {
                        copy = word.substr(2);
                        return Hexadecimal(copy, line_number);
                } else if (copy.at(1) == 'B') {
                        copy = word.substr(2);
                        return Binary(copy, line_number);
                } else {
                        return Decimal(word, line_number);
                }
        case '#':
                copy = word.substr(1);
                // FALLTHROUGH
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
                        copy = word.substr(1);
                        return Hexadecimal(copy);
                }
                return Label(word, line_number);
        case 'R':
                if (copy.length() < 2) {
                        break;
                }
                if (std::isdigit(copy.at(1))) {
                        if (copy.length() < 3 || std::isspace(copy.at(2)) || copy.at(2) == ',') {
                                return Register(word);
                        }
                }
                break;
        default:break;
        }

        if (copy == "ADD") {
                return Add(word, line_number);
        } else if (copy == "AND") {
                return And(word, line_number);
        } else if (copy == "NOT") {
                return Not(word, line_number);
        } else if (copy == "JSR") {
                return Jsr(word, line_number);
        } else if (copy == "JSRR") {
                return Jsrr(word, line_number);
        } else if (copy == "JMP") {
                return Jmp(word, line_number);
        } else if (copy == "ST") {
                return St(word, line_number);
        } else if (copy == "STR") {
                return Str(word, line_number);
        } else if (copy == "STI") {
                return Sti(word, line_number);
        } else if (copy == "LD") {
                return Ld(word, line_number);
        } else if (copy == "LEA") {
                return Lea(word, line_number);
        } else if (copy == "LDI") {
                return Ldi(word, line_number);
        } else if (copy == "LDR") {
                return Ldr(word, line_number);
        } else if (copy == "PUTS") {
                return Puts(word, line_number);
        } else if (copy == "PUTSP") {
                return Putsp(word, line_number);
        } else if (copy == "HALT") {
                return Halt(word, line_number);
        } else if (copy == "TRAP") {
                return Trap(word, line_number);
        } else if (copy == "GETC") {
                return Getc(word, line_number);
        } else if (copy == "OUT") {
                return Out(word, line_number);
        } else if (copy == "IN") {
                return In(word, line_number);
        }

        if (copy.length() < 6 && copy.compare(0, 2, "BR")) {
                if (copy.length() == 3) {
                        if (copy.at(2) == 'N') {
                                return Br(word, line_number, true);
                        } else if (copy.at(2) == 'Z') {
                                return Br(word, line_number, false, true);
                        } else if (copy.at(2) == 'P') {
                                return Br(word, line_number, false, false, true);
                        }
                } else if (copy.length() == 4) {
                        if (copy.compare(2, 2, "NZ")) {
                                return Br(word, line_number, true, true);
                        } else if (copy.compare(2, 2, "ZP")) {
                                return Br(word, line_number, false, true, true);
                        } else if (copy.compare(2, 2, "NP")) {
                                return Br(word, line_number, true, true, true);
                        }
                } else if (copy.length() == 5 && copy.compare(2, 3, "NZP")) {
                        return Br(word, line_number, true, true, true);
                }
        }

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
