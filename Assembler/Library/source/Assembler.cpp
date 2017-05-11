#include "Assembler.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "../../../cxxopts.hpp"
#include "Lexer.hpp"

Assembler::Assembler()
        : symbols(), files_to_assemble(), as_assembled(), m_logger(), tokens()
{

}

Assembler::Assembler(int argument_count, char **arguments)
        : Assembler()
{
        cxxopts::Options parser(arguments[0], "An LC3 Assembler");
        parser.add_options()
                      ("h,help", "Print this help message")
                      ("q,quiet", "Don't print anything (other than errors)")
                      ("w,warn", "Warning level. One of the following:                            - all"
                               "                                                       - syntax"
                               "                                                    - ignore"
                               "                                                    - multiple"
                               "                                                  - none"
                               "                                                      - logic"
                               "                                                 These"
                               " can be provided together seperated by a comma, e.g."
                               " --warn multiple,syntax                                  ",
                       cxxopts::value<std::string>()->default_value("all"))
                      ("n,no-warn", "Turn off all warnings")
                      ("pretend", "Don't write the assembled instructions to any files");

        try {
                parser.parse(argument_count, arguments);
        } catch (const cxxopts::OptionException &e) {
                std::cout << "Error parsing options: " << e.what() << '\n' << parser.help() << '\n';
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
                m_logger.set_warning_level(warning_level);
        }

        if (parser.count("quiet")) {
                quiet = true;
                m_logger.set_quietness(quiet);
        }

        if (parser.count("pretend")) {
                do_write = false;
        }

        if (parser.count("no-warn")) {
                warning_level = Logger::WARNING_TYPE::NONE;
                m_logger.set_warning_level(warning_level);
        }

        for (int i = 1; i < argument_count; ++i) {
                files_to_assemble.push_back(std::string(arguments[i]));
        }
}

Assembler::Assembler(std::string &file)
        : Assembler()
{
        files_to_assemble.emplace_back(file);
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
 * The first pass of the assembler generates the symbol table.
 */
void Assembler::do_first_pass()
{
        // TODO: As it is, if there is no .ORIG directive as the first instruction in the file,
        // TODO: we still look at the rest of the file. Is it smarter to stop if we don't see it
        // TODO: first? Or should we just default to setting the memory address to 0x3000?

        std::int32_t memory_required = 0;

        const auto &memory_requirement_of = [this](const auto &token, auto &&t_tokens) -> std::int32_t
        {
                if (!origin_seen) {
                        token->expected(".ORIG statement");
                        return -1;
                } else if (end_seen) {
                        std::stringstream stream;
                        stream << token->token << " after .END directive, it will be ignored.";
                        m_logger.LOG(Logger::WARNING,
                                     token->at_line,
                                     token->file,
                                     stream.str(),
                                     Logger::WARNING_TYPE::IGNORED);
                        return 0;
                } else if (token->valid_arguments(t_tokens)) {
                        return token->guess_memory_size(t_tokens);
                } else {
                        return -1;
                }
        };

        for (auto &&tokenized_line : tokens) {
                switch (tokenized_line.front()->type()) {
                case Token::DIR_ORIG:
                        if (origin_seen) {
                                m_logger.LOG(Logger::ERROR,
                                             tokenized_line.front()->at_line,
                                             tokenized_line.front()->file,
                                             "Redefinition of Origin memory address.",
                                             Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS);
                                ++error_count;
                                break;
                        }
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
                                        std::stringstream stream;
                                        if (symbol.second.address == internal_program_counter) {
                                                stream << "Multiple labels found for address 0x" << std::uppercase
                                                       <<std::hex << internal_program_counter
                                                       << "\nNOTE: \t Previous label '" << symbol.first
                                                       << "' found on line " << std::dec
                                                       << symbol.second.line_number << '.';
                                                m_logger.LOG(Logger::WARNING,
                                                             tokenized_line.front()->at_line,
                                                             tokenized_line.front()->file,
                                                             stream.str(),
                                                             Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS);
                                                break;
                                        }
                                }

                                if (symbols.count(tokenized_line.front()->token)) {
                                        std::stringstream stream;
                                        stream << "Multiple definitions of label '"
                                               << tokenized_line.front()->token
                                               << "'\nNOTE: \t Label was first defined on line "
                                               << symbols.at(tokenized_line.front()->token).line_number << '.';
                                        m_logger.LOG(Logger::ERROR,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     stream.str(),
                                                     Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS);
                                        ++error_count;
                                }

                                symbols.insert(std::pair<std::string, Symbol>(
                                        tokenized_line.front()->token,
                                        Symbol(internal_program_counter, tokenized_line.front()->at_line))
                                );

                                longest_symbol_length = std::max(
                                        longest_symbol_length,
                                        static_cast<int>(tokenized_line.front()->token.length())
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
}

/**
 * The second pass of the assembler actually assembles the program.
 */
void Assembler::do_second_pass()
{
        std::int32_t memory_required = 0;

        for (auto &&tokenized_line : tokens) {
                if (tokenized_line.front()->type() == Token::DIR_END) {
                        end_seen = true;
                        break;
                }

                memory_required = tokenized_line.front()->assemble(tokenized_line, symbols, internal_program_counter);

                if (memory_required < 0) {
                        error_count += static_cast<std::size_t>(-memory_required);
                } else {
                        internal_program_counter += static_cast<std::uint16_t>(memory_required);
                }
        }
}

/**
 * Generate the machine code for the file.
 *
 * @return The machine code gathered into a vector.
 */
void Assembler::generate_machine_code()
{
        if (as_assembled.size()) {
                // We've probably already been here.
                return;
        }

        for (const auto &tokenized_line : tokens) {
                for (const auto &assembled_line : tokenized_line.front()->as_assembled()) {
                        if (tokenized_line.front()->type() == Token::OP_BR) {
                                if ((assembled_line & 0xFE00) == (as_assembled.back() & 0xFE00)) {
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     "Statement before this one checks for the same condition code."
                                                             " This might mean this one will never execute.",
                                                     Logger::WARNING_TYPE::LOGIC);
                                } else if (!(assembled_line & 0xFF)) {
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     "BR with an offset of 0 will probably do nothing.",
                                                     Logger::WARNING_TYPE::LOGIC);
                                } else if ((assembled_line & 0x1FF) == 0x1FF) {
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     "BR with an offset of -1 will probably cause an infinite loop.",
                                                     Logger::WARNING_TYPE::LOGIC);
                                }
                        } else if (tokenized_line.front()->type() == Token::OP_JSR) {
                                if (!(assembled_line & 0x7FF)) {
                                        // Technically, this isn't true. It could just be a way to get the
                                        // current value of the PC into R7, but it's still worth a warning.
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     "JSR with an offset of 0 will probably do nothing.",
                                                     Logger::WARNING_TYPE::LOGIC);
                                } else if ((assembled_line & 0x7FF) == 0x7FF) {
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     "JSR with an offset of -1 will probably cause an infinite loop.",
                                                     Logger::WARNING_TYPE::LOGIC);
                                }
                        } else if (tokenized_line.front()->type() == Token::OP_TRAP) {
                                if ((assembled_line & 0x00FF) > 0x0025 || (assembled_line & 0x00FF) < 0x0020) {
                                        std::stringstream stream;
                                        stream << "TRAP was supplied a trap vector of " << (assembled_line & 0x00FF)
                                               << ", which is possibly an illegal trap vector.\n";
                                        m_logger.LOG(Logger::WARNING,
                                                     tokenized_line.front()->at_line,
                                                     tokenized_line.front()->file,
                                                     stream.str(),
                                                     Logger::WARNING_TYPE::LOGIC);
                                }
                        }

                        as_assembled.push_back(assembled_line);
                }
        }
}

bool Assembler::assemble()
{
        if (files_to_assemble.empty()) {
                std::cerr << "No files to assemble\n";
                return false;
        }

        std::stringstream stream;

        bool all_fine = true;

        for (const auto &file : files_to_assemble) {
                stream.str(std::string());
                stream << "\n --- Assembling " << file << " ---\n\nStarting first pass\n";
                m_logger.LOG(Logger::MESSAGE, 0, file, stream.str(), Logger::NONE);

                Lexer lexer(file, quiet, warning_level);
                error_count = lexer.parse_into(tokens);

                do_first_pass();

                if (!origin_seen) {
                        m_logger.LOG(Logger::ERROR,
                                     0,
                                     file,
                                     "No .ORIG directive. (Is the file empty?)",
                                     Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS);
                        ++error_count;
                } else if (!end_seen) {
                        m_logger.LOG(Logger::ERROR,
                                     0,
                                     file,
                                     "Reached the end of the file, and found no .END directive.",
                                     Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS);
                        ++error_count;
                }

                stream.str(std::string());
                stream << error_count << " error" << (error_count == 1 ? "" : "'s") << " found on the first pass\n";
                m_logger.LOG(Logger::MESSAGE, 0, file, stream.str(), Logger::NONE);

                if (error_count || tokens.empty()) {
                        all_fine = false;
                } else {  // No point in this if the first pass failed.
                        internal_program_counter = file_memory_origin_address;

                        stream.str(std::string());
                        m_logger.LOG(Logger::MESSAGE, 0, file, "Starting second pass\n", Logger::NONE);

                        do_second_pass();

                        stream.str(std::string());
                        stream << error_count << " error" << (error_count == 1 ? "" : "'s")
                               << " found on the second pass\n";
                        m_logger.LOG(Logger::MESSAGE, 0, file, stream.str(), Logger::NONE);

                        if (!error_count) {
                                if (do_write) {
                                        generate_machine_code();
                                        write(file.substr(0, file.find_first_of('.')));
                                }
                        }
                }

                reset();
        }

        return all_fine && error_count == 0;
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

        for (const auto instruction : as_assembled) {
                object_file.put(static_cast<char>((instruction >> 8) & 0xFF));
                object_file.put(static_cast<char>(instruction & 0xFF));

                binary_file << std::bitset<16>(instruction).to_string() << '\n';

                hex_file << std::setfill('0') << std::uppercase << std::hex << std::setw(4)
                         << instruction << '\n';
        }

        symbol_file << "// Symbol table\n" << "// Scope Level 0:\n" << "//\t" << std::left << std::setfill(' ')
                    << std::setw(longest_symbol_length) << "Symbol Name" << " Page Address\n//\t"
                    << std::setfill('-') << std::setw(longest_symbol_length) << '-' << " ------------\n";

        for (const auto &symbol : symbols) {
                symbol_file << "//\t" << std::setfill(' ') << std::setw(longest_symbol_length)
                            << symbol.first << ' ' << std::uppercase << std::hex << std::setfill('0')
                            << std::setw(4) << symbol.second.address << '\n';
        }

        const auto &symbol_at = [this](const std::uint16_t address)
        {
                return std::find_if(
                        symbols.cbegin(), symbols.cend(),
                        [address](const auto &sym) -> bool
                        {
                                return sym.second.address == address;
                        }
                );
        };

        std::uint16_t pc = 0;

        std::map<std::string, Symbol>::const_iterator symbol;

        const std::string empty = " ";

        for (auto &tokenized_line : tokens) {
                symbol = symbol_at(pc);

                lst_file << tokenized_line.front()->disassemble(
                        pc, symbol == symbols.cend() ? empty : symbol->first, longest_symbol_length
                );

                if (tokenized_line.front()->type() == Token::DIR_END) {
                        break;
                }
        }
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
                        return sym.second.address == (offset + pc + 1);
                });
        };

        std::map<std::string, Symbol>::const_iterator symbol;

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
                stream << "LEA R" << ((instruction & 0x0E00) >> 9) << ", ";
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << symbol->first;
                } else if (instruction & 0x100) {
                        stream << "#-" << std::dec << (-instruction & 0xFF);
                } else {
                        stream << '#' << std::dec << (instruction & 0xFF);
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
                stream << "STI R" << ((instruction & 0x0E00) >> 9) << ", ";
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << symbol->first;
                } else if (instruction & 0x100) {
                        stream << "#-" << std::dec << (-instruction & 0xFF);
                } else {
                        stream << '#' << std::dec << (instruction & 0xFF);
                }
                break;
        case 0xA000:
                stream << "LDI R" << ((instruction & 0x0E00) >> 9) << ", ";
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << symbol->first;
                } else if (instruction & 0x100) {
                        stream << "#-" << std::dec << (-instruction & 0xFF);
                } else {
                        stream << '#' << std::dec << (instruction & 0xFF);
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
                if (instruction & 0x0800) {
                        stream << "JSR ";
                        if ((symbol = find_symbol(5)) != symbols.cend()) {
                                stream << symbol->first;
                        } else if (instruction & 0x700) {
                                stream << "#-" << std::dec << (-instruction & 0x3FF);
                        } else {
                                stream << '#' << std::dec << (instruction & 0x3FF);
                        }
                } else {
                        stream << ".FILL 0x" << std::uppercase << std::setw(4) << std::setfill('0') << std::hex
                               << instruction;
                }
                break;
        case 0x3000:
                stream << "ST R" << ((instruction & 0x0E00) >> 9) << ", ";
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << symbol->first;
                } else if (instruction & 0x100) {
                        stream << "#-" << std::dec << (-instruction & 0xFF);
                } else {
                        stream << '#' << std::dec << (instruction & 0xFF);
                }
                break;
        case 0x2000:
                stream << "LD R" << ((instruction & 0x0E00) >> 9) << ", ";
                if ((symbol = find_symbol(7)) != symbols.cend()) {
                        stream << symbol->first;
                } else if (instruction & 0x100) {
                        stream << "#-" << std::dec << (-instruction & 0xFF);
                } else {
                        stream << '#' << std::dec << (instruction & 0xFF);
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
                if (instruction & 0x0E00) {
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

                        stream << ' ';

                        if ((symbol = find_symbol(7)) != symbols.cend()) {
                                stream << symbol->first;
                        } else if (instruction & 0x100) {
                                stream << "#-" << std::dec << (instruction & 0xFF);
                        } else {
                                stream << '#' << std::dec << (instruction & 0xFF);
                        }
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
                warning_level = Logger::WARNING_TYPE::NONE;
        } else if (warning == "syntax") {
                warning_level |= Logger::WARNING_TYPE::SYNTAX;
        } else if (warning == "ignore") {
                warning_level |= Logger::WARNING_TYPE::IGNORED;
        } else if (warning == "multiple") {
                warning_level |= Logger::WARNING_TYPE::MULTIPLE_DEFINITIONS;
        } else if (warning == "logic") {
                warning_level |= Logger::WARNING_TYPE::LOGIC;
        } else if (warning == "all") {
                warning_level = Logger::WARNING_TYPE::ALL;
        } else {
                std::cerr << "Argument --warn expects one (or more) of the following:" << '\n'
                          << "\t- all\n"
                          << "\t- syntax\n"
                          << "\t- ignore\n"
                          << "\t- multiple\n"
                          << "\t- logic\n"
                          << "\t- none\n"
                          << "Got '" << warning << "'\n";
                exit(EXIT_FAILURE);
        }
}
