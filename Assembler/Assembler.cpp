#include "Assembler.hpp"

#include <iostream>

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "cxxopts.hpp"

static Assembler::Assembler *assembler = nullptr;
static std::string          last_error;

bool Assembler::init()
{
        assembler = new ::Assembler::Assembler();

        if (!assembler) {
                last_error = "Assembler allocation failed";
                return false;
        }

        return true;
}

void Assembler::de_init()
{
        if (assembler) {
                delete assembler;
        }
}

int ::Assembler::run(int argc, char **args)
{
        if (!init()) {
                std::perror(last_error.c_str());
                return 1;  // No point in continuing if initialisation failed.
        }

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

                if (option_parser.count("help")) {
                        std::cout << option_parser.help() << '\n';
                        de_init();
                        return 0;
                }

                if (option_parser.count("include")) {
                        auto      &&dirs = option_parser["include"].as<std::vector<std::string>>();
                        for (auto &&dir : dirs) {
                                Config::config_add_search_directory(dir);
                        }
                }

                if (option_parser.count("stop")) {
                        Config::config_set(Config::CONFIG_OPTIONS::STOP_ON_FIRST_ERR);
                }

                if (option_parser.count("error")) {
                        Config::config_set(Config::CONFIG_OPTIONS::WARN_AS_ERROR);
                }

                if (option_parser.count("warn")) {
                        // TODO: Implement (as above).
                }

        } catch (const cxxopts::OptionException& e)
        {
                std::cout << "LC3AS Command Line Error: " << e.what() << '\n';
                return 1;
        }

        // TODO: Move this to when the Assembler finishes.
        /*
        Diagnostics::Diagnostic_Message m(
                "Random Message",
                Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                Diagnostics::DIAGNOSTIC::WARNING
        );

        m.provide_context(Diagnostics::Diagnostic_Context());

        Diagnostics::push(m);

        Diagnostics::push(
                Diagnostics::Diagnostic_Message(
                        "Another message",
                        Diagnostics::DIAGNOSTIC_TYPE::SPELLING,
                        Diagnostics::DIAGNOSTIC::ERROR
                )
        );
        */

        Diagnostics::unwind();

        de_init();

        return 0;
}

Assembler::Assembler::Assembler()
{

}
