#include "Assembler.hpp"

#include <iostream>

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "cxxopts.hpp"

int Assembler::assemble(int argc, char **args)
{
        std::ios_base::sync_with_stdio(false);

        Assembler assembler;
        if (!assembler.configure(argc, args)) {
                return 1;
        }

        // TODO: Move this to when the Assembler finishes.

        Diagnostics::Diagnostic m {
                Diagnostics::FileContext(
                        Diagnostics::Variant<std::string>("Assembler.cpp", Console::FOREGROUND_COLOUR::YELLOW),
                        Diagnostics::Variant<std::size_t>(44, Console::FOREGROUND_COLOUR::YELLOW),
                        Diagnostics::Variant<std::size_t>(60, Console::FOREGROUND_COLOUR::YELLOW)
                ), "Incorrect spelling", Diagnostics::DIAGNOSTIC_TYPE::SPELLING, Diagnostics::DIAGNOSTIC::WARNING
        };

        m.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>("Assembler.cpp",
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<std::size_t>(44, Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<std::size_t>(60, Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                '^',
                                "Dictionary does not contain word 'seet'; Did you mean 'set'?",
                                "\tSome rather long line that contains an invalid word called seet"
                        ), '~', 4, "set"
                )
        );

        Diagnostics::push(m);

        m = Diagnostics::Diagnostic(
                Diagnostics::FileContext("Assembler.cpp", 44, 64),
                "Invalid punctuation",
                Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                Diagnostics::DIAGNOSTIC::ERROR
        );

        m.provide_context(
                std::make_unique<Diagnostics::SelectionContext>(
                        Diagnostics::FileContext(
                                Diagnostics::Variant<std::string>("Assembler.cpp",
                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<std::size_t>(44, Console::FOREGROUND_COLOUR::YELLOW),
                                Diagnostics::Variant<std::size_t>(64, Console::FOREGROUND_COLOUR::YELLOW)
                        ),
                        '^',
                        "Missing punctuation at end of sentence",
                        "\tSome rather long line that contains an invalid word called seet",
                        "(.!?)"
                )
        );

        Diagnostics::push(m);

        if (Config::find_path("/Users/pyxxil/Sync/Projects/LC3/Assembler/Assembler.cpp").empty()) {
                m = Diagnostics::Diagnostic(
                        Diagnostics::FileContext("Assembler.cpp", 69, 31),
                        "File '/Users/pyxxil/Sync/Projects/LC3/Assembler/Assembler.cpp' not found",
                        Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                        Diagnostics::DIAGNOSTIC::ERROR
                );

                m.provide_context(std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(Diagnostics::Variant<std::string>("Assembler.cpp",
                                                                                           Console::FOREGROUND_COLOUR::YELLOW),
                                                         Diagnostics::Variant<std::size_t>(69,
                                                                                           Console::FOREGROUND_COLOUR::YELLOW),
                                                         Diagnostics::Variant<std::size_t>(31,
                                                                                           Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                '^',
                                "Include found here",
                                "        if (Config::find_path(\"/Users/pyxxil/Sync/Projects/LC3/Assembler/Assembler.cpp\").empty()) {"
                        ), '~', 55
                ));

                Diagnostics::push(m);
        }

        const std::string s { "../Assembler/Assembler.cpp" };
        if (Config::find_path(s).empty()) {
                m = Diagnostics::Diagnostic(
                        Diagnostics::FileContext("Assembler.cpp", 92, 32),
                        "File '" + s + "' not found",
                        Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                        Diagnostics::DIAGNOSTIC::ERROR
                );

                m.provide_context(std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>("Assembler.cpp",
                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<std::size_t>(92, Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<std::size_t>(31, Console::FOREGROUND_COLOUR::YELLOW)
                                ),
                                '^',
                                "Include found here",
                                "        const std::string s { \"../Assembler/Assembler.cpp\" };"
                        ), '~', s.length()
                ));

                Diagnostics::push(m);
        }

        assembler.assemble();

        Diagnostics::unwind();

        return 0;
}

Assembler::Assembler::Assembler()
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
                                     ("files", "Files to assemble", cxxopts::value<std::vector<std::string>>());

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
                auto &&files = option_parser["files"].as<std::vector<std::string>>();
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

        return true;
}

void Assembler::Assembler::assemble()
{

}
