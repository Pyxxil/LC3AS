#include "Diagnostics.hpp"

#include <iostream>

void Diagnostics::unwind()
{
        while (!diagnostics_log.empty()) {
                std::cout << Diagnostics::pop().to_string() << '\n';
        }
}

Diagnostics::Diagnostic_Message &Diagnostics::pop()
{
        auto &&message = Diagnostics::diagnostics_log.front();
        diagnostics_log.pop();
        return message;
}

void Diagnostics::push(Diagnostic_Message &&message)
{
        diagnostics_log.emplace(message);
}

void Diagnostics::push(Diagnostic_Message &message)
{
        diagnostics_log.emplace(message);
}
