#include "Assembler.hpp"

#include <fstream>
#include <iomanip>

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "cxxopts.hpp"
#include "Parser.hpp"

int Assembler::assemble(int argc, char **args)
{
        std::ios_base::sync_with_stdio(false);

        Assembler assembler;
        if (!assembler.configure(argc, args)) {
                return 1;
        }

        assembler.assemble();

        return 0;
}

Assembler::Assembler::Assembler()
        : files_to_assemble()
{

}

bool Assembler::Assembler::configure(int argc, char **args)
{
        cxxopts::Options option_parser("LC3AS", "LC-3 Assembly Language Assembler");
        try {
                option_parser.positional_help("<assembly files>");
                option_parser.add_options()
                                     ("h,help", "Print this help message")
                                     ("I,include",
                                      "Add a directory to search for .INCLUDE directives",
                                      cxxopts::value<std::vector<std::string>>(),
                                      "DIRECTORY")
                                     ("s,stop", "Stop on the first error")
                                     ("e,error", "Report warnings as errors")
                                     ("w,warn", "Choose warning type") // TODO: Fix this
                                     ("files", "Files to assemble", cxxopts::value<std::vector<std::string>>())
                                     ("q,quiet", "Be quiet");

                // TODO: Add a --attempt-fix. Basically, assume that a problem is meant to be fixed to what we
                // TODO: think it should, e.g. when we encounter a single '/', the assembler keeps going because
                // TODO: it assumes that it was meant to be a double '//', indiccation a comment. Do the same thing
                // TODO: for character literals and strings -- basically, anything left on that line will be treated
                // TODO: as a string/character (which, of course, would lead to problems with character literals).
                // TODO:    - Maybe only use 1 character for character literals, and anything else as extra?

                option_parser.parse_positional("files");
                option_parser.parse(argc, args);
        } catch (const cxxopts::OptionException &e) {
                std::cout << "LC3AS Command Line Error: " << e.what() << '\n';
                return false;
        }

        Config::add_search_directory(".", "Current Running Directory");

        if (option_parser.count("help")) {
                std::cout << option_parser.help() << '\n';
                return false;
        }

        if (!option_parser.count("files")) {
                std::cout << "LC3AS: No input files.\n";
                return false;
        } else {
                auto      &&files = option_parser["files"].as<std::vector<std::string>>();
                for (auto &&file : files) {
                        files_to_assemble.emplace_back(file);
                }
        }

        if (option_parser.count("include")) {
                auto      &&dirs = option_parser["include"].as<std::vector<std::string>>();
                for (auto &&dir : dirs) {
                        Config::add_search_directory(dir);
                }
        }

        if (option_parser.count("stop")) {
                Config::set(Config::CONFIG_OPTIONS::STOP_ON_FIRST_ERR);
        }

        if (option_parser.count("error")) {
                Config::set(Config::CONFIG_OPTIONS::WARN_AS_ERROR);
        }

        if (option_parser.count("warn")) {
                // TODO: Implement (as above).
        }

        if (option_parser.count("quiet")) {
                Config::set(Config::CONFIG_OPTIONS::BE_QUIET);
        }

        return true;
}

void Assembler::Assembler::assemble()
{
        for (auto &&file : files_to_assemble) {
                std::ifstream f(file);

                if (f.fail()) {
                        // We can't do anything with a file that failed to open for some reason...
                        Console::write_line(Diagnostics::Variant<std::string>("Unable to open file '" + file + "'"));
                        f.close();
                        continue;
                } else {
                        f.close();
                }

                if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
                        Console::write("--- ");
                        Console::write(Diagnostics::Variant<std::string>("Assembling file '" + file + "'"));
                        Console::write(" ---\n");
                }

                Parser parser(file);
                if (!parser.parse()) {
                        // No errors, so let's write to the associated files.
                        tokens  = parser.parsed_tokens();
                        symbols = parser.parsed_symbols();

                        symbol_padding = parser.symbol_padding();

                        generate_machine_code();

                        if (!Diagnostics::critical()) {
                                write(file);
                        }

                        assembled.clear();
                }

                if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
                        Diagnostics::unwind();
                } else {
                        Diagnostics::reset();
                }
        }
}

void Assembler::Assembler::generate_machine_code()
{
        for (size_t i = 0; i < tokens.size(); ++i) {
                auto &&tokenized_line = tokens[i];

                for (const auto &assembled_line : tokenized_line.front()->as_assembled()) {
                        if (!Config::is_set(Config::CONFIG_OPTIONS::BE_QUIET)) {
                                // Not really much point in checking for these if the user wants us to be quiet.
                                check_and_mark_warnings(tokenized_line, assembled_line, i);
                        }
                        assembled.push_back(assembled_line);
                }
        }
}

void Assembler::Assembler::check_and_mark_warnings(const std::vector<std::shared_ptr<Token>> &tokenized_line,
                                                   uint16_t assembled_line,
                                                   size_t i)
{
        if (tokenized_line.front()->type() == Token::OP_BR ||
            tokenized_line.front()->type() == Token::OP_JSR) {
                if (tokenized_line.front()->type() == Token::OP_BR &&
                    ((assembled_line & 0xFE00) == (assembled.back() & 0xFE00))) {
                        Diagnostics::Diagnostic diag(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(tokenized_line.front()->file,
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                     Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                "Statement before this one checks for the same condition code.",
                                Diagnostics::LOGIC,
                                Diagnostics::WARNING
                        );

                        diag.provide_context(
                                std::make_unique<Diagnostics::HighlightContext>(
                                        Diagnostics::SelectionContext(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<
                                                                std::string>(tokenized_line.front()->file,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line.front()->at_line,
                                                                        Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line.front()->at_column,
                                                                        Console::FOREGROUND_COLOUR::YELLOW)
                                                ), '^', "This might mean this line is superfluous",
                                                std::string(
                                                        lexed_lines[tokenized_line.front()->file].at(
                                                                tokenized_line.front()->at_line - 1
                                                        )
                                                )
                                        ), '~', tokenized_line.front()->token.length()
                                )
                        );

                        diag.provide_context(
                                std::make_unique<Diagnostics::HighlightContext>(
                                        Diagnostics::SelectionContext(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<
                                                                std::string>(tokens[i -
                                                                                    1].front()->file,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokens[i - 1].front()->at_line,
                                                                        Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokens[i -
                                                                               1].front()->at_column,
                                                                        Console::FOREGROUND_COLOUR::YELLOW)
                                                ), '^', "Checks the same condition code as this line",
                                                std::string(
                                                        lexed_lines[tokens[i - 1].front()->file].at(
                                                                tokens[i - 1].front()->at_line - 1
                                                        )
                                                )
                                        ), '~', tokens[i - 1].front()->token.length()
                                )
                        );

                        Diagnostics::push(diag);
                }

                if (!(assembled_line & 0x1FF)) {
                        Diagnostics::Diagnostic diag(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(tokenized_line.front()->file,
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                     Console::FOREGROUND_COLOUR::YELLOW)
                                ), "Superfluous statement", Diagnostics::LOGIC, Diagnostics::WARNING
                        );

                        diag.provide_context(
                                std::make_unique<Diagnostics::HighlightContext>(
                                        Diagnostics::SelectionContext(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<
                                                                std::string>(tokenized_line[1]->file,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_line,
                                                                        Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_column,
                                                                        Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                '^',
                                                "Offset of 0 is superfluous -- simulator will go to next line no matter what",
                                                std::string(
                                                        lexed_lines[tokenized_line[1]->file].at(
                                                                tokenized_line[1]->at_line - 1
                                                        )
                                                )
                                        ), '~', tokenized_line[1]->token.length()
                                )
                        );

                        if (tokenized_line[1]->type() == Token::LABEL) {
                                auto &&sym = symbols.find(tokenized_line[1]->token);
                                diag.provide_context(
                                        std::make_unique<Diagnostics::HighlightContext>(
                                                Diagnostics::SelectionContext(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<
                                                                        std::string>(sym->second.file,
                                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<
                                                                        size_t>(sym->second.line_number,
                                                                                Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<
                                                                        size_t>(sym->second.column,
                                                                                Console::FOREGROUND_COLOUR::YELLOW)
                                                        ), '^', "Referred to label defined here",
                                                        std::string(
                                                                lexed_lines[sym->second.file].at(
                                                                        sym->second.line_number - 1
                                                                )
                                                        )
                                                ), '~', sym->first.length()
                                        )
                                );
                        }

                        Diagnostics::push(diag);
                } else if ((assembled_line & 0x1FF) == 0x1FF) {
                        Diagnostics::Diagnostic diag(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(tokenized_line.front()->file,
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                     Console::FOREGROUND_COLOUR::YELLOW)
                                ), "Possible infinite loop", Diagnostics::LOGIC, Diagnostics::WARNING
                        );

                        diag.provide_context(
                                std::make_unique<Diagnostics::HighlightContext>(
                                        Diagnostics::SelectionContext(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<
                                                                std::string>(tokenized_line[1]->file,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_line,
                                                                        Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_column,
                                                                        Console::FOREGROUND_COLOUR::YELLOW)
                                                ), '^', "Offset of -1 might cause an infinite loop",
                                                std::string(
                                                        lexed_lines[tokenized_line[1]->file].at(
                                                                tokenized_line[1]->at_line - 1
                                                        )
                                                )
                                        ), '~', tokenized_line[1]->token.length()
                                )
                        );

                        if (tokenized_line[1]->type() == Token::LABEL) {
                                auto &&sym = symbols.find(tokenized_line[1]->token);
                                diag.provide_context(
                                        std::make_unique<Diagnostics::HighlightContext>(
                                                Diagnostics::SelectionContext(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<
                                                                        std::string>(sym->second.file,
                                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<
                                                                        size_t>(sym->second.line_number,
                                                                                Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<
                                                                        size_t>(sym->second.column,
                                                                                Console::FOREGROUND_COLOUR::YELLOW)
                                                        ), '^', "Referred to label defined here",
                                                        std::string(
                                                                lexed_lines[sym->second.file].at(
                                                                        sym->second.line_number - 1
                                                                )
                                                        )
                                                ), '~', sym->first.length()
                                        )
                                );
                        }

                        Diagnostics::push(diag);
                }
        }

        if (tokenized_line.front()->type() == Token::OP_TRAP) {
                if ((assembled_line & 0x00FF) > 0x0025 || (assembled_line & 0x00FF) < 0x0020) {
                        Diagnostics::Diagnostic diag(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(tokenized_line.front()->file,
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                     Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                     Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                "Possible illegal trap vector",
                                Diagnostics::LOGIC,
                                Diagnostics::WARNING
                        );

                        diag.provide_context(
                                std::make_unique<Diagnostics::HighlightContext>(
                                        Diagnostics::SelectionContext(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<
                                                                std::string>(tokenized_line[1]->file,
                                                                             Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_line,
                                                                        Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<
                                                                size_t>(tokenized_line[1]->at_column,
                                                                        Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                '^',
                                                "Trap vector not between 0x20 and 0x25 (inclusive) might cause"
                                                        " an illegal trap vector exception",
                                                std::string(lexed_lines[tokenized_line[1]->file].at(
                                                        tokenized_line[1]->at_line - 1)
                                                )
                                        ), '~', tokenized_line[1]->token.length(), "0x2{0..5}"
                                )
                        );

                        Diagnostics::push(diag);
                }
        }
}

void Assembler::Assembler::write(const std::string &file)
{
        const std::string prefix = file.substr(0, file.find_last_of('.'));

        std::ofstream object_file(prefix + ".obj", std::ofstream::binary);
        std::ofstream binary_file(prefix + ".bin");
        std::ofstream symbol_file(prefix + ".sym");
        std::ofstream hex_file(prefix + ".hex");
        std::ofstream lst_file(prefix + ".lst");

        for (const auto instruction : assembled) {
                object_file.put(static_cast<char>((instruction >> 8) & 0xFF));
                object_file.put(static_cast<char>(instruction & 0xFF));

                binary_file << std::bitset<16>(instruction).to_string() << '\n';

                hex_file << std::setfill('0') << std::uppercase << std::hex << std::setw(4)
                         << instruction << '\n';
        }

        symbol_file << "// Symbol table\n" << "// Scope Level 0:\n" << "//\t" << std::left << std::setfill(' ')
                    << std::setw(symbol_padding) << "Symbol Name" << " Page Address\n//\t"
                    << std::setfill('-') << std::setw(symbol_padding) << '-' << " ------------\n";

        for (auto &&symbol : symbols) {
                symbol_file << "//\t" << std::setfill(' ') << std::setw(symbol_padding)
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

        for (auto &&tokenized_line : tokens) {
                symbol = symbol_at(pc);

                if (symbol == symbols.cend()) {
                        lst_file << tokenized_line.front()->disassemble(
                                pc, empty, symbol_padding
                        );
                } else {
                        lst_file << tokenized_line.front()->disassemble(
                                pc, symbol->first, symbol_padding
                        );
                }

                if (tokenized_line.front()->type() == Token::DIR_END) {
                        break;
                }
        }
}
