#include <iostream>

#include "Assembler.hpp"
#include "Diagnostics.hpp"

int main(int argc, char **argv)
{
        if (!Assembler::init()) {
                // TODO: Change this so that init() creates some sort of string that says what went wrong,
                // TODO: which can then be used here.
                std::cout << Assembler::last_error << '\n';
                return 1;
        }

        Diagnostics::push(
                Diagnostics::Diagnostic_Message(
                        "Random Message",
                        Diagnostics::DIAGNOSTIC_TYPE::SYNTAX,
                        Diagnostics::DIAGNOSTIC::WARNING
                )
        );

        Diagnostics::push(
                Diagnostics::Diagnostic_Message(
                        "Another message",
                        Diagnostics::DIAGNOSTIC_TYPE::SPELLING,
                        Diagnostics::DIAGNOSTIC::ERROR
                )
        );

        Diagnostics::unwind();

        Assembler::de_init();

        return 0;
}