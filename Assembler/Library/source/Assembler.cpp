#include "Assembler.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>

#include "Tokens/All_Tokens.hpp"

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

        std::size_t first_char_on_directive = length > 1 ? static_cast<std::size_t>(*(string + 1)) : 0;

        for (std::size_t index = 0; index < length; ++index) {
                if (*string == '.') {
                        if (*(string + index) == '.') {
                                _hash = (_hash * hashed_letters[first_char_on_directive - 0x41u]) ^
                                        (first_char_on_directive * hashed_letters[first_char_on_directive - 0x41u]);
                        } else {
                                _hash = (_hash * hashed_letters[static_cast<std::size_t>(*(string + index)) - 0x41u]) ^
                                        (first_char_on_directive *
                                         hashed_letters[static_cast<std::size_t>(*(string + index)) - 0x41u]);
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
          , symbols()
          , as_assembled()
          , tokens()
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
                perror(fileName.c_str());
                exit(EXIT_FAILURE);
        }

        std::string line;

        std::vector<std::shared_ptr<Token>> tokenized_line;

        for (int line_number = 1; std::getline(file, line); line_number++) {
                if (line.empty()) {
                        continue;
                }

                tokenized_line = tokenizeLine(line, line_number);
                if (!tokenized_line.empty()) {
                        tokens.push_back(tokenized_line);
                }
        }

        return tokens;
}

/**
 * Add a token to the current line's tokens.
 *
 * @param token The string containing the token.
 * @param t_tokens The current tokens in the line.
 * @param line_number The line number (only relevant for working with files).
 */
void Assembler::addToken(std::string &token, std::vector<std::shared_ptr<Token>> &t_tokens, int line_number)
{
        if (!token.empty()) {
                t_tokens.push_back(tokenize(token, line_number));
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
std::vector<std::shared_ptr<Token>> Assembler::tokenizeLine(std::string &line, int line_number)
{
        std::string current;

        std::vector<std::shared_ptr<Token>> tokenized_line;

        for (std::size_t index = 0; index < line.length();) {
                char character = line.at(index);

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        // However, it does mean we want to check what we just got.
                        addToken(current, tokenized_line, line_number);
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
                                std::cerr << "WARNING: ";
                                if (line_number) {
                                        std::cerr << "Line " << std::dec << line_number << ": ";
                                }
                                std::cerr << "Expected '//', but found '/'. "
                                          << "Treating it as if it's '//' (i.e. comment)\n";
                        }
                        break;
                } else if (character == ',' || character == ':') {
                        addToken(current, tokenized_line, line_number);
                } else if (character == '"'
#ifdef INCLUDE_ADDONS
                           || character == '\''
#endif
                        ) {
                        addToken(current, tokenized_line, line_number);

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
                                tokenized_line.push_back(std::make_shared<Token>(current, line_number));
                                tokenized_line.back()->unterminated(
#ifdef INCLUDE_ADDONS
                                        terminator == '\'' ? "character" :
#endif
                                        "string"
                                );
                                ++error_count;
                        } else if (terminator == '"') {
                                tokenized_line.push_back(std::make_shared<String>(current, line_number));
#ifdef INCLUDE_ADDONS
                        } else {
                                tokenized_line.push_back(std::make_shared<Character>(current, line_number));
#endif
                        }

                        current.erase();
                } else {
                        current += character;
                }
                ++index;
        }

        const bool any_non_space_characters = std::any_of(
                current.cbegin(), current.cend(),
                [](unsigned char chr) -> bool
                {
                        return !std::isspace(chr);
                }
        );

        if (any_non_space_characters) {
                addToken(current, tokenized_line, line_number);
        }

        return tokenized_line;
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
        case hash("RET", 3):
                return std::make_shared<Ret>(word, line_number);
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
                return std::make_shared<Br>(word, line_number, true, true, true);
        case hash("BRN", 3):
                return std::make_shared<Br>(word, line_number, true, false, false);
        case hash("BRZ", 3):
                return std::make_shared<Br>(word, line_number, false, true, false);
        case hash("BRP", 3):
                return std::make_shared<Br>(word, line_number, false, false, true);
        case hash("BRNZ", 4):
                // FALLTHROUGH
        case hash("BRZN", 4):
                return std::make_shared<Br>(word, line_number, true, true, false);
        case hash("BRNP", 4):
                // FALLTHROUGH
        case hash("BRPN", 4):
                return std::make_shared<Br>(word, line_number, true, false, true);
        case hash("BRZP", 4):
                // FALLTHROUGH
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
#ifdef INCLUDE_ADDONS
        case hash(".NEG", 4):
                return std::make_shared<Neg>(word, line_number);
        case hash(".SUB", 4):
                return std::make_shared<Sub>(word, line_number);
#endif
        default:
                // Of course, if it doesn't match the above, then we'll treat it as a label.
                break;
        }

        const std::regex decimal("#?-?\\d+");
        const std::regex binary("-?0?[bB][0-1]+");
        const std::regex hexadecimal("0?[xX][0-9a-fA-F]+");
        const std::regex octal("\\\\[0-7]+");
        const std::regex _register("[rR]\\d");
        const std::regex label("[a-zA-Z0-9_]+");

        if (std::regex_match(word, decimal)) {
                return std::make_shared<Decimal>(word, line_number);
        } else if (std::regex_match(word, binary)) {
                return std::make_shared<Binary>(word, line_number);
        } else if (std::regex_match(word, hexadecimal)) {
                return std::make_shared<Hexadecimal>(word, line_number);
        } else if (std::regex_match(word, octal)) {
                return std::make_shared<Octal>(word, line_number);
        } else if (std::regex_match(word, _register)) {
                return std::make_shared<Register>(word, line_number);
        } else if (std::regex_match(word, label)) {
                return std::make_shared<Label>(word, line_number);
        } else {
                const std::shared_ptr<Token> token = std::make_shared<Token>(word, line_number);
                token->is_valid = false;
                std::cerr << "ERROR: ";
                if (line_number) {
                        std::cerr << "Line " << std::dec << line_number << ": ";
                }
                std::cerr << "Expected one of: label, instruction, or immediate value. Found '"
                          << word << "' instead.\n";
                ++error_count;
                return token;
        }
}

/**
 * The first pass of the assembler generates the symbol table.
 */
void Assembler::do_first_pass()
{
        std::int32_t memory_required = 0;

        const auto memory_requirement_of = [this](const auto &token, auto &&t_tokens) -> std::int32_t
        {
                if (!origin_seen) {
                        token->expected(".ORIG statement");
                        return -1;
                } else if (end_seen) {
                        std::cerr << "WARNING: ";
                        if (token->at_line) {
                                std::cerr << "Line " << std::dec
                                          << token->at_line
                                          << ":\n";
                        }
                        std::cerr << token->word
                                  << " after .END directive, it will be ignored.";
                        return 0;
                } else if (token->valid_arguments(t_tokens)) {
                        return token->guess_memory_size(t_tokens);
                } else {
                        return -1;
                }
        };

        for (auto &tokenized_line : tokens) {
                switch (tokenized_line.front()->type()) {
                case Token::DIR_ORIG:
                        origin_seen     = true;
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += memory_required;
                        } else {
                                error_count += static_cast<std::size_t>(-memory_required);
                        }
                        break;
                case Token::LABEL:
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                std::static_pointer_cast<Label>(tokenized_line.front())->address =
                                        internal_program_counter;

                                for (const auto &symbol : symbols) {
                                        if (symbol.second->word == tokenized_line.front()->word) {
                                                std::cerr << "ERROR: Line " << tokenized_line.front()->at_line
                                                          << ": Multiple definitions of label '"
                                                          << tokenized_line.front()->word
                                                          << "'\nNOTE: \tLabel was first defined on line "
                                                          << symbol.second->at_line
                                                          << '\n';
                                                ++error_count;
                                                break;
                                        }
                                }

                                if (symbols.count(internal_program_counter)) {
                                        std::cerr << "WARNING: Multiple labels found for address 0x"
                                                  << std::hex << internal_program_counter
                                                  << "\nNOTE: \tPrevious label '"
                                                  << symbols.at(internal_program_counter)->word
                                                  << "' found on line " << std::dec
                                                  << symbols.at(internal_program_counter)->at_line
                                                  << '\n';
                                }

                                symbols.insert(std::pair<std::uint16_t, std::shared_ptr<Label>>(
                                        internal_program_counter,
                                        std::static_pointer_cast<Label>(tokenized_line.front()))
                                );

                                longest_symbol_length = std::max(longest_symbol_length,
                                                                 tokenized_line.front()->word.length());

                                internal_program_counter += memory_required;
                        } else {
                                error_count += static_cast<std::size_t>(-memory_required);
                        }
                        break;
                case Token::DIR_END:
                        end_seen = true;
                        if (!tokenized_line.front()->valid_arguments(tokenized_line)) {
                                ++error_count;
                        }
                        break;
                default:
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += memory_required;
                        } else {
                                error_count += static_cast<std::size_t>(-memory_required);
                        }
                        break;
                }
        }

        if (!end_seen) {
                std::cerr << "Reached the end of the file, and found no .END directive\n";
        }
}

/**
 * The second pass of the assembler actually assembles the program.
 */
void Assembler::do_second_pass()
{
        std::int32_t memory_required = 0;

        puts("Starting second pass");

        for (auto &tokenized_line : tokens) {
                memory_required = tokenized_line.front()->assemble(tokenized_line, *this);

                if (memory_required < 0) {
                        error_count += static_cast<std::size_t>(-memory_required);
                } else {
                        internal_program_counter += static_cast<std::uint16_t>(memory_required);
                }
        }

        std::cout << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the second pass\n";
}

/**
 * Generate the machine code for the file.
 *
 * @return The machine code gathered into a vector.
 */
std::vector<std::uint16_t> Assembler::generate_machine_code()
{
        if (as_assembled.size()) {
                return as_assembled;
        }

        for (const auto &tokenized_line : tokens) {
                for (const auto &assembled_line : tokenized_line.front()->as_assembled()) {
                        as_assembled.push_back(assembled_line);
                }
        }

        return as_assembled;
}

bool Assembler::assemble(std::string &fileName)
{
        puts("Starting first pass");

        tokenizeFile(fileName);

        do_first_pass();

        std::cout << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the first pass\n";

        if (error_count || tokens.empty()) {
                return false;
        }

        internal_program_counter = file_memory_origin_address;

        do_second_pass();

        if (!error_count) {
                generate_machine_code();
                write(fileName.substr(0, fileName.find_first_of('.')));
        }

        return error_count == 0;
}

/**
 * Write the instructions to all relevant files.
 *
 * symbol file (.sym): The symbols found in the file.
 * hexadecimal file (.hex): The hexadecimal representation of each instruction.
 * binary file (.bin): The binary representation of each instruction.
 * object file (.obj): The machine code of each instruction.
 * list file (.lst): Each instruction with it's address, hex representation, binary representation, line number,
 *                   and the string representation.
 *
 * @param prefix The file's prefix to use as the prefix for each file above.
 */
void Assembler::write(std::string &prefix)
{
        if (!as_assembled.size()) {
                return;
        }

        std::ofstream object_file(prefix + ".obj", std::ofstream::binary);
        std::ofstream binary_file(prefix + ".bin");
        std::ofstream symbol_file(prefix + ".sym");
        std::ofstream hex_file(prefix + ".hex");
        std::ofstream lst_file(prefix + ".lst");

        const auto write_list = [&lst_file, this](
                const std::uint16_t instruction, const std::uint16_t program_counter,
                const std::size_t line_number, const auto &label, const auto &disassembled
        )
        {
                // Address
                lst_file << '(' << std::uppercase << std::setfill('0') << std::setw(4)
                         << std::hex << program_counter << ')'
                         // Hexadecimal representation
                         << ' ' << std::uppercase << std::setfill('0') << std::setw(4)
                         << std::hex << std::right << instruction
                         // Binary representation
                         << ' ' << std::bitset<16>(instruction) << ' '
                         // Line number
                         << '(' << std::setfill(' ') << std::right << std::setw(4)
                         << std::dec << line_number << ')'
                         // Label (if any)
                         << ' ' << std::setfill(' ')
                         << std::setw(static_cast<int>(longest_symbol_length)) << std::left << label
                         // Disassembled instruction
                         << ' ' << disassembled << '\n';
        };

        std::size_t   line = 1;
        std::uint16_t pc   = 0;

        auto        &&symbol              = symbols.cbegin();
        std::size_t instruction_index     = 0;
        std::size_t instruction_end_index = as_assembled.size();

        symbol_file << "// Symbol table\n"
                    << "// Scope Level 0:\n"
                    << "//\t" << std::left << std::setw(static_cast<int>(longest_symbol_length))
                    << "Symbol Name" << " Page Address\n//\t"
                    << std::setw(static_cast<int>(longest_symbol_length))
                    << std::setfill('-') << '-' << " ------------\n";

        std::stringstream ss;
        ss << ".ORIG 0x" << std::hex << as_assembled.at(instruction_index);
        write_list(as_assembled.at(instruction_index), pc, line, ' ', ss.str());

        object_file.put(static_cast<char>((as_assembled.at(instruction_index) >> 8) & 0xFF));
        object_file.put(static_cast<char>(as_assembled.at(instruction_index) & 0xFF));

        binary_file << std::bitset<16>(as_assembled.at(instruction_index)).to_string() << '\n';

        hex_file << std::setfill('0') << std::setw(4) << std::uppercase << std::hex
                 << as_assembled.at(instruction_index) << '\n';

        pc = as_assembled.at(instruction_index);

        // TODO: Is it worth it to figure out if there's a .BLKW over using a lot of .FILL's?
        for (++instruction_index, ++line; instruction_index < instruction_end_index || symbol != symbols.cend();
             ++instruction_index, ++pc, ++line) {

                object_file.put(static_cast<char>((as_assembled.at(instruction_index) >> 8) & 0xFF));
                object_file.put(static_cast<char>(as_assembled.at(instruction_index) & 0xFF));

                binary_file << std::bitset<16>(as_assembled.at(instruction_index)).to_string() << '\n';

                hex_file << std::setfill('0') << std::setw(4) << std::uppercase << std::hex
                         << as_assembled.at(instruction_index) << '\n';

                if (symbol != symbols.cend() && symbol->first == pc) {
                        write_list(as_assembled.at(instruction_index), pc, line, symbol->second->word,
                                   disassemble(as_assembled.at(instruction_index), pc));

                        symbol_file << "//\t" << std::left
                                    << std::setw(static_cast<int>(longest_symbol_length))
                                    << std::setfill(' ') << symbol->second->word << ' ' << std::uppercase
                                    << std::hex << symbol->first << '\n';

                        ++symbol;
                } else {
                        write_list(as_assembled.at(instruction_index), pc, line, ' ',
                                   disassemble(as_assembled.at(instruction_index), pc));
                }
        }

        write_list(0u, 0u, line, ' ', ".END");
}

/**
 * Disassemble an instruction into it's corresponding parts.
 *
 * @param instruction The instruction to disassemble.
 * @param pc The current program counter
 * @return The string representation of the instruction.
 */
std::string Assembler::disassemble(std::uint16_t instruction, std::uint16_t pc)
{
        std::stringstream stream;

        const auto &&find_symbol = [this, instruction, pc](const std::uint8_t shift)
        {
                return std::find_if(
                        symbols.cbegin(), symbols.cend(),
                        [instruction, pc, shift](const auto &sym) -> bool
                        {
                                const std::int16_t offset = static_cast<std::int16_t>(
                                        static_cast<std::int16_t>(instruction << shift) >> shift);
                                return sym.first == (offset + pc + 1);
                        }
                );
        };

        std::map<std::uint16_t, std::shared_ptr<Label>>::const_iterator symbol;

        switch (instruction & 0xF000) {
        case 0xF000:
                if (instruction & 0x0F00) {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                } else {
                        switch (instruction & 0xFF) {
                        case 0x20:
                                stream << "GETC";
                                break;
                        case 0x21:
                                stream << "OUT";
                                break;
                        case 0x22:
                                stream << "PUTS";
                                break;
                        case 0x23:
                                stream << "IN";
                                break;
                        case 0x24:
                                stream << "PUTSP";
                                break;
                        case 0x25:
                                stream << "HALT";
                                break;
                        default:
                                stream << "TRAP 0x" << std::uppercase << std::setw(2)
                                       << std::setfill('0') << std::hex << (instruction & 0xFF);
                                break;
                        }
                }
                break;
        case 0xE000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LEA R" << ((instruction & 0x0E00) >> 9)
                               << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0xD000:
                stream << ".FILL 0x" << std::uppercase << std::setw(4)
                       << std::setfill('0') << std::hex << instruction;
                break;
        case 0xC000:
                if (instruction == 0xC1C0) {
                        stream << "RET";
                } else if (!(instruction & 0x0E3F)) {
                        stream << "JMP R" << ((instruction & 0x01C0) >> 6);
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0xB000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "STI R" << ((instruction & 0x0E00) >> 9)
                               << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0xA000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LDI R" << ((instruction & 0x0E00) >> 9)
                               << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0x9000:
                if ((instruction & 0x003F) != 0x003F) {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                } else {
                        stream << "NOT R" << ((instruction & 0x0E00) >> 9) << ", R"
                               << ((instruction & 0x01C0) >> 6);
                }
                break;
        case 0x8000:
                stream << ".FILL 0x" << std::uppercase << std::setw(4)
                       << std::setfill('0') << std::hex << instruction;
                break;
        case 0x7000:
                stream << "STR R" << ((instruction & 0x0E00) >> 9) << ", R"
                       << ((instruction & 0x01C0) >> 6) << ", #"
                       << (static_cast<std::int16_t>(instruction << 10) >> 10);
                break;
        case 0x6000:
                stream << "LDR R" << ((instruction & 0x0E00) >> 9) << ", R"
                       << ((instruction & 0x01C0) >> 6) << ", #" << std::dec
                       << (static_cast<std::int16_t>(instruction << 10) >> 10);
                break;
        case 0x5000:
                stream << "AND R" << ((instruction & 0x0E00) >> 9) << ", R"
                       << ((instruction & 0x01C0) >> 6);
                if (instruction & 0x20) {
                        stream << ", #" << std::dec
                               << (static_cast<std::int16_t>(instruction << 11) >> 11);
                } else {
                        stream << ", R" << (instruction & 0x7);
                }
                break;
        case 0x4000:
                if ((instruction & 0x0800) && (symbol = find_symbol(5)) != symbols.cend()) {
                        stream << "JSR " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0x3000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "ST R" << ((instruction & 0x0E00) >> 9)
                               << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0x2000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LD R" << ((instruction & 0x0E00) >> 9)
                               << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        case 0x1000:
                stream << "ADD R" << ((instruction & 0x0E00) >> 9) << ", R"
                       << ((instruction & 0x01C0) >> 6);
                if (instruction & 0x20) {
                        stream << ", #" << std::dec
                               << (static_cast<std::int16_t>(instruction << 11) >> 11);
                } else {
                        stream << ", R" << (instruction & 0x7);
                }
                break;
        case 0x0000:
        default:
                if ((instruction & 0x0E00) && (symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "BR";

                        if (instruction & 0x0800) {
                                stream << 'n';
                        }

                        if (instruction & 0x0400) {
                                stream << 'z';
                        }

                        if (instruction & 0x0200) {
                                stream << 'p';
                        }

                        stream << ' ' << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4)
                               << std::setfill('0') << std::hex << instruction;
                }
                break;
        }

        return stream.str();
}
