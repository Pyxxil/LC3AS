#include "Assembler.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>

#include "../../../cxxopts.hpp"
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

Assembler::Assembler()
        : error_count(0), internal_program_counter(0), origin_seen(false), end_seen(false), symbols()
          , files_to_assemble(), as_assembled(), tokens()
{

}

Assembler::Assembler(int argument_count, char **arguments)
        : Assembler()
{
        cxxopts::Options parser(arguments[0], "An LC3 Assembler");
        parser.add_options()
                      ("h,help", "Print this help message")
                      ("q,quiet", "Don't print anything (other than errors)")
                      ("w,warn", "Warning level, One of the following:                            - all"
                               "                                                       - syntax"
                               "                                                    - ignore"
                               "                                                    - multiple"
                               "                                                  - none",
                       cxxopts::value<std::string>()->default_value("none"))
                      ("n,nowarn", "Turn off all warnings");

        try {
                parser.parse(argument_count, arguments);
        } catch (const cxxopts::OptionException &e) {
                std::cout << "Error parsing options: " << e.what() << '\n';
                std::cout << parser.help() << '\n';
                exit(EXIT_FAILURE);
        }

        if (parser.count("help")) {
                std::cout << parser.help() << '\n';
                exit(EXIT_SUCCESS);
        }

        if (parser.count("warn")) {
                std::string warning_value = parser["warn"].as<std::string>();
                std::string token;

                std::size_t pos = 0;

                while ((pos = warning_value.find(',')) != std::string::npos) {
                        token = warning_value.substr(0, pos);
                        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

                        change_warning_level(token);

                        warning_value.erase(0, pos + 1);
                }

                change_warning_level(warning_value);
        }

        if (parser.count("quiet")) {
                quiet = true;
        }

        if (parser.count("nowarn")) {
                warning_level = NONE;
        }

        for (int i = 1; i < argument_count; ++i) {
                files_to_assemble.push_back(std::string(arguments[i]));
        }
}

Assembler::Assembler(std::string &file)
        : Assembler()
{
        files_to_assemble.push_back(file);
}

Assembler::Assembler(std::string &&file)
        : Assembler(file)
{

}

void Assembler::reset()
{
        longest_symbol_length = 20;
        error_count           = 0;

        origin_seen = false;
        end_seen    = false;

        file_memory_origin_address = 0;
        internal_program_counter   = 0;

        as_assembled.clear();
        symbols.clear();
        tokens.clear();
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

        char terminated_by = 0;

        for (std::size_t index = 0; index < line.length();) {
                char character = line.at(index);

                if (std::isspace(character)) {
                        // We don't care about space characters.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        // However, it does mean we want to check what we just got.
                        addToken(current, tokenized_line, line_number);
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
                                WARN(SYNTAX, line_number, "Expected '//', but found '/'. "
                                        "Treating it as if it's '//' (i.e. comment)");
                        }
                        break;
                } else if (character == ',') {
                        if (!tokenized_line.size() || terminated_by) {
                                WARN(SYNTAX, line_number, "Extraneous comma");
                        }
                        addToken(current, tokenized_line, line_number);
                        terminated_by = ',';
                } else if (character == ':') {
                        if (tokenized_line.size() > 1 || !current.size() || terminated_by) {
                                WARN(SYNTAX, line_number, "Extraneous colon");
                        }
                        addToken(current, tokenized_line, line_number);
                        terminated_by = ':';
#ifdef INCLUDE_ADDONS
                } else if (character == '"' || character == '\'') {
#else
                } else if (character == '"') {
#endif
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
#ifdef INCLUDE_ADDONS
                                tokenized_line.back()->unterminated(terminator == '\'' ? "character" : "string");
#else
                                tokenized_line.back()->unterminated("string");
#endif
                                ++error_count;
                                tokenized_line.clear();
                                return tokenized_line;
#ifdef INCLUDE_ADDONS
                        } else if (terminator == '\'') {
                                tokenized_line.push_back(std::make_shared<Character>(current, line_number));
#endif
                        } else {
                                tokenized_line.push_back(std::make_shared<String>(current, line_number));
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

        if (hashed) {
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
                std::stringstream stream;
                stream << "Expected one of: label, instruction, or immediate value. Found '" << word << "' instead";
                ERR(line_number, stream.str());
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
                        std::stringstream stream;
                        stream << token->word << " after .END directive, it will be ignored";
                        this->WARN(IGNORED, token->at_line, stream.str());
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
                                                std::stringstream stream;
                                                stream << "Multiple definitions of label '"
                                                       << tokenized_line.front()->word
                                                       << "'\nNOTE: \tLabel was first defined on line "
                                                       << symbol.second->at_line;
                                                ERR(tokenized_line.front()->at_line, stream.str());
                                                break;
                                        }
                                }

                                if (symbols.count(internal_program_counter)) {
                                        std::stringstream stream;
                                        stream << "Multiple labels found for address 0x" << std::hex
                                               << internal_program_counter << "\nNOTE: \tPrevious label '"
                                               << symbols.at(internal_program_counter)->word << "' found on line "
                                               << std::dec << symbols.at(internal_program_counter)->at_line;
                                        WARN(MULTIPLE_DEFINITIONS, tokenized_line.front()->at_line, stream.str());
                                }

                                symbols.insert(std::pair<std::uint16_t,
                                                         std::shared_ptr<Label>>(internal_program_counter,
                                                                                 std::static_pointer_cast<Label>(
                                                                                         tokenized_line.front())));

                                longest_symbol_length = std::max(
                                        longest_symbol_length,
                                        static_cast<int>(tokenized_line.front()->word.length())
                                );

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

        LOG(MESSAGE, "Starting second pass\n");

        for (auto &tokenized_line : tokens) {
                memory_required = tokenized_line.front()->assemble(tokenized_line, *this);

                if (memory_required < 0) {
                        error_count += static_cast<std::size_t>(-memory_required);
                } else {
                        internal_program_counter += static_cast<std::uint16_t>(memory_required);
                }
        }

        std::stringstream stream;
        stream << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the second pass\n";
        LOG(MESSAGE, stream.str());
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

bool Assembler::assemble()
{
        if (files_to_assemble.empty()) {
                std::cout << "No files to assemble\n";
                return false;
        }

        std::stringstream stream;

        if (files_to_assemble.size() == 1) {
                LOG(MESSAGE, "Starting first pass\n");

                tokenizeFile(files_to_assemble.at(0));

                do_first_pass();

                stream << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the first pass\n";
                LOG(MESSAGE, stream.str());

                if (error_count || tokens.empty()) {
                        return false;
                }

                internal_program_counter = file_memory_origin_address;

                do_second_pass();

                if (!error_count) {
                        generate_machine_code();

                        write(files_to_assemble.at(0).substr(0, files_to_assemble.at(0).find_first_of('.')));
                }

                return true;
        }

        for (auto &file : files_to_assemble) {
                stream.str(std::string());
                stream << "\n --- Assembling " << file << " ---\n\n";
                LOG(MESSAGE, stream.str());

                LOG(MESSAGE, "Starting first pass\n");

                tokenizeFile(file);

                do_first_pass();

                stream.str(std::string());
                stream << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the first pass\n";
                LOG(MESSAGE, stream.str());

                if (error_count || tokens.empty()) {
                        return false;
                }

                internal_program_counter = file_memory_origin_address;

                do_second_pass();

                if (!error_count) {
                        generate_machine_code();
                        write(file.substr(0, file.find_first_of('.')));
                }

                reset();
        }

        return error_count == 0;
}

bool Assembler::assemble(std::string &fileName)
{
        files_to_assemble.clear();
        files_to_assemble.push_back(fileName);

        return assemble();
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

        const auto write_list = [&lst_file, this](const std::uint16_t instruction, const std::uint16_t program_counter,
                                                  const std::size_t line_number, const auto &label,
                                                  const auto &disassembled)
        {
                lst_file
                         // Address
                         << '(' << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << program_counter
                         << ") "
                         // Hexadecimal representation
                         << std::uppercase << std::hex << std::right << std::setfill('0') << std::setw(4) << instruction
                         // Binary representation
                         << ' ' << std::bitset<16>(instruction) << ' '
                         // Line number
                         << '(' << std::setfill(' ') << std::right << std::dec << std::setw(4) << line_number << ')'
                         // Label (if any)
                         << ' ' << std::setfill(' ') << std::setw(longest_symbol_length) << std::left << label
                         // Disassembled instruction
                         << ' ' << disassembled << '\n';
        };

        std::size_t   line = 1;
        std::uint16_t pc   = 0;

        auto        &&symbol              = symbols.cbegin();
        std::size_t instruction_index     = 0;
        std::size_t instruction_end_index = as_assembled.size();

        symbol_file << "// Symbol table\n" << "// Scope Level 0:\n" << "//\t" << std::left << std::setfill(' ')
                    << std::setw(longest_symbol_length) << "Symbol Name" << " Page Address\n//\t"
                    << std::setfill('-') << std::setw(longest_symbol_length) << '-' << " ------------\n";

        std::stringstream ss;
        ss << ".ORIG 0x" << std::hex << as_assembled.at(instruction_index);
        write_list(as_assembled.at(instruction_index), pc, line, ' ', ss.str());

        object_file.put(static_cast<char>((as_assembled.at(instruction_index) >> 8) & 0xFF));
        object_file.put(static_cast<char>(as_assembled.at(instruction_index) & 0xFF));

        binary_file << std::bitset<16>(as_assembled.at(instruction_index)).to_string() << '\n';

        hex_file << std::setfill('0') << std::uppercase << std::hex << std::setw(4)
                 << as_assembled.at(instruction_index) << '\n';

        pc = as_assembled.at(instruction_index);

        // TODO: Is it worth it to figure out if there's a .BLKW over using a lot of .FILL's?
        for (++instruction_index, ++line; instruction_index < instruction_end_index || symbol != symbols.cend();
             ++instruction_index, ++pc, ++line) {

                object_file.put(static_cast<char>((as_assembled.at(instruction_index) >> 8) & 0xFF));
                object_file.put(static_cast<char>(as_assembled.at(instruction_index) & 0xFF));

                binary_file << std::bitset<16>(as_assembled.at(instruction_index)).to_string() << '\n';

                hex_file << std::setfill('0') << std::uppercase << std::hex << std::setw(4)
                         << as_assembled.at(instruction_index) << '\n';

                if (symbol != symbols.cend() && symbol->first == pc) {
                        write_list(as_assembled.at(instruction_index), pc, line,
                                   symbol->second->word, disassemble(as_assembled.at(instruction_index), pc));

                        symbol_file << "//\t" << std::left << std::setfill(' ') << std::setw(longest_symbol_length)
                                    << symbol->second->word << ' ' << std::uppercase << std::hex << symbol->first << '\n';

                        ++symbol;
                } else {
                        write_list(as_assembled.at(instruction_index), pc, line,
                                   ' ', disassemble(as_assembled.at(instruction_index), pc));
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
                return std::find_if(symbols.cbegin(), symbols.cend(), [instruction, pc, shift](const auto &sym) -> bool
                {
                        const std::int16_t offset = static_cast<std::int16_t>(
                                static_cast<std::int16_t>(instruction << shift) >> shift);
                        return sym.first == (offset + pc + 1);
                });
        };

        std::map<std::uint16_t, std::shared_ptr<Label>>::const_iterator symbol;

        switch (instruction & 0xF000) {
        case 0xF000:
                if (instruction & 0x0F00) {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
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
                                stream << "TRAP 0x" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex
                                       << (instruction & 0xFF);
                                break;
                        }
                }
                break;
        case 0xE000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LEA R" << ((instruction & 0x0E00) >> 9) << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0xD000:
                stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << instruction;
                break;
        case 0xC000:
                if (instruction == 0xC1C0) {
                        stream << "RET";
                } else if (!(instruction & 0x0E3F)) {
                        stream << "JMP R" << ((instruction & 0x01C0) >> 6);
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0xB000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "STI R" << ((instruction & 0x0E00) >> 9) << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0xA000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LDI R" << ((instruction & 0x0E00) >> 9) << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0x9000:
                if ((instruction & 0x003F) != 0x003F) {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                } else {
                        stream << "NOT R" << ((instruction & 0x0E00) >> 9) << ", R" << ((instruction & 0x01C0) >> 6);
                }
                break;
        case 0x8000:
                stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex << instruction;
                break;
        case 0x7000:
                stream << "STR R" << ((instruction & 0x0E00) >> 9) << ", R" << ((instruction & 0x01C0) >> 6) << ", #"
                       << (static_cast<std::int16_t>(instruction << 10) >> 10);
                break;
        case 0x6000:
                stream << "LDR R" << ((instruction & 0x0E00) >> 9) << ", R" << ((instruction & 0x01C0) >> 6) << ", #"
                       << std::dec << (static_cast<std::int16_t>(instruction << 10) >> 10);
                break;
        case 0x5000:
                stream << "AND R" << ((instruction & 0x0E00) >> 9) << ", R" << ((instruction & 0x01C0) >> 6);
                if (instruction & 0x20) {
                        stream << ", #" << std::dec << (static_cast<std::int16_t>(instruction << 11) >> 11);
                } else {
                        stream << ", R" << (instruction & 0x7);
                }
                break;
        case 0x4000:
                if ((instruction & 0x0800) && (symbol = find_symbol(5)) != symbols.cend()) {
                        stream << "JSR " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0x3000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "ST R" << ((instruction & 0x0E00) >> 9) << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0x2000:
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << "LD R" << ((instruction & 0x0E00) >> 9) << ", " << symbol->second->word;
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0x1000:
                stream << "ADD R" << ((instruction & 0x0E00) >> 9) << ", R" << ((instruction & 0x01C0) >> 6);
                if (instruction & 0x20) {
                        stream << ", #" << std::dec << (static_cast<std::int16_t>(instruction << 11) >> 11);
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
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        }

        return stream.str();
}

void Assembler::change_warning_level(std::string &warning)
{
        if (warning == "none") {
                warning_level = WARNING_LEVEL::NONE;
        } else if (warning == "syntax") {
                warning_level |= WARNING_LEVEL::SYNTAX;
        } else if (warning == "ignore") {
                warning_level |= WARNING_LEVEL::IGNORED;
        } else if (warning == "multiple") {
                warning_level |= WARNING_LEVEL::MULTIPLE_DEFINITIONS;
        } else if (warning == "all") {
                warning_level = ALL;
        } else {
                std::cerr << "Argument --warn expects one (or more) of the following:" << '\n' << "\t- none" << '\n'
                          << "\t- syntax" << '\n' << "\t- ignore" << '\n' << "\t- multiple" << '\n' << "\t- all" << '\n'
                          << "Got '" << warning << "'\n";
                exit(EXIT_FAILURE);
        }
}

void Assembler::LOG(Assembler::LOGGING_TYPE level, std::string &&message)
{
        if (we_should_be_quiet()) {
                return;
        }

        if (level == ERROR || level == WARNING) {
                std::cerr << message;
        } else {
                std::cout << message;
        }
}

void Assembler::WARN(WARNING_LEVEL level, int line_number, std::string &&warning)
{
        if (!(warning_level & level)) {
                return;
        }

        std::stringstream stream;

        stream <<  "WARNING: ";

        if (line_number) {
                stream << "Line " << line_number << ": ";
        }

        stream << warning << ".\n";

        LOG(WARNING, stream.str());
}

void Assembler::ERR(int line_number, std::string &&error)
{
        std::stringstream stream;

        stream << "ERROR: ";

        if (line_number) {
                stream << "Line " << line_number << ": ";
        }

        stream << error << ".\n";

        ++error_count;

        LOG(ERROR, stream.str());
}
