#include "Diagnostics.hpp"

static std::queue<Diagnostics::Diagnostic_Message> diagnostics_log;

static const Console::Colour diagnostic_colours[] = {
        Console::Colour(Console::FOREGROUND_COLOUR::CYAN),
        Console::Colour(Console::FOREGROUND_COLOUR::RED),
        Console::Colour(Console::FOREGROUND_COLOUR::MAGENTA),
};

static Diagnostics::Diagnostic_Message &pop()
{
        auto &&message = diagnostics_log.front();
        diagnostics_log.pop();
        return message;
}

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

void Diagnostics::push(Diagnostic_Message &&message)
{
        diagnostics_log.emplace(message);
}

void Diagnostics::push(Diagnostic_Message &message)
{
        diagnostics_log.emplace(message);
}

std::size_t Diagnostics::count()
{
        return diagnostics_log.size();
}
