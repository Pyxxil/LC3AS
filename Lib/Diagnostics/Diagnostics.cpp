#include "Diagnostics.hpp"

#include <algorithm>

static std::deque<Diagnostics::Diagnostic> diagnostics_log;

const Diagnostics::diagnostic_type Diagnostics::diagnostic_colours[] = {
        Diagnostics::diagnostic_type(
                "Warning",
                Console::Colour(
                        Console::FOREGROUND_COLOUR::CYAN,
                        Console::MODIFIER::UNDERLINE
                )
        )
        , Diagnostics::diagnostic_type(
                "Error",
                Console::Colour(
                        Console::FOREGROUND_COLOUR::RED,
                        Console::MODIFIER::UNDERLINE
                )
        )
        , Diagnostics::diagnostic_type(
                "Note",
                Console::Colour(
                        Console::FOREGROUND_COLOUR::BLACK,
                        Console::MODIFIER::BOLD
                )
        )
};

void Diagnostics::unwind()
{
        while (!diagnostics_log.empty()) {
                auto &&diagnostic = diagnostics_log.front();
                Console::write(diagnostic);
                diagnostics_log.pop_front();
        }
}

void Diagnostics::push(Diagnostic &&message)
{
        diagnostics_log.emplace_back(message);
}

void Diagnostics::push(Diagnostic &message)
{
        diagnostics_log.emplace_back(message);
}

bool Diagnostics::critical()
{
        return std::any_of(
                diagnostics_log.cbegin(), diagnostics_log.cend(),
                [](auto &&diag)
                {
                        return diag.is_critical();
                }
        );
}

size_t Diagnostics::count()
{
        return diagnostics_log.size();
}

Diagnostics::Diagnostic::Diagnostic(Diagnostics::FileContext file,
                                    std::string t_message,
                                    Diagnostics::DIAGNOSTIC_TYPE t_type,
                                    Diagnostics::DIAGNOSTIC diagnostic)
        : message(std::move(t_message)), d_type(t_type), d(diagnostic), info(std::move(file))
{ }

Diagnostics::Diagnostic::Diagnostic(Diagnostics::Diagnostic &other)
        : message(other.message), d_type(other.d_type), d(other.d), context(std::move(other.context))
          , info(other.info)
{

}

Diagnostics::Diagnostic::Diagnostic(Diagnostics::Diagnostic &&other) noexcept
        : message(other.message), d_type(other.d_type), d(other.d), context(std::move(other.context))
          , info(other.info)
{

}

Diagnostics::Diagnostic &Diagnostics::Diagnostic::operator =(Diagnostics::Diagnostic &&rhs) noexcept
{
        if (&rhs != this) {
                message = rhs.message;
                d       = rhs.d;
                d_type  = rhs.d_type;
                context = std::move(rhs.context);
        }
        return *this;
}

std::ostream &Diagnostics::Diagnostic::write_to(std::ostream &os) const
{
        info.write_to(os) << ' ' << diagnostic_colours[type()] << ": " << message << '\n';

        for (auto &&context_item : context) {
                Console::write_line(*context_item);
        }

        return os;
}

void Diagnostics::reset()
{
        diagnostics_log.clear();
}
