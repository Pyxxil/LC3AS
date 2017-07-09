#include <iostream>

#include "Assembler.hpp"
#include "Diagnostics.hpp"

int main(int argc, char **argv)
{
        if (!Assembler::init()) {
                std::cout << Assembler::last_error << '\n';
                return 1;  // No point in continuing if initialisation failed.
        }

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

        Diagnostics::unwind();

        Assembler::de_init();

        return 0;
}