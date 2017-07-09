#include "Diagnostics.hpp"

static const Console::Colour diagnostic_colours[] = {
        Console::Colour(Console::FOREGROUND_COLOUR::CYAN),
        Console::Colour(Console::FOREGROUND_COLOUR::RED),
        Console::Colour(Console::FOREGROUND_COLOUR::MAGENTA),
};

void Diagnostics::unwind()
{
        while (!diagnostics_log.empty()) {
                auto &&diagnostic = pop();

                Console::write(diagnostic.to_string(), diagnostic_colours[static_cast<std::size_t>(diagnostic.type())]);

                if (diagnostic.has_context()) {
                        // TODO: Print it's context too.
                        Console::write(diagnostic.given_context());
                }

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
