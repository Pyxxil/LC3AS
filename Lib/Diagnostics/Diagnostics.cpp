#include "Diagnostics.hpp"

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

Diagnostics::Diagnostic::Diagnostic(const Diagnostics::FileContext &file)
        : message("Something happened..."), d_type(), d(), context(), info(file)
{ }

Diagnostics::Diagnostic::Diagnostic(const Diagnostics::FileContext &file,
                                    std::string &&t_message,
                                    Diagnostics::DIAGNOSTIC_TYPE t_type,
                                    Diagnostics::DIAGNOSTIC diagnostic)
        : message(t_message), d_type(t_type), d(diagnostic), context(), info(file)
{ }

Diagnostics::Diagnostic::Diagnostic(Diagnostics::Diagnostic &other)
        : message(other.message), d_type(other.d_type), d(other.d), context(std::move(other.context))
          , info(other.info)
{

}

Diagnostics::Diagnostic &Diagnostics::Diagnostic::operator =(Diagnostics::Diagnostic &&rhs)
{
        if (&rhs != this) {
                message = rhs.message;
                d       = rhs.d;
                d_type  = rhs.d_type;
                context = std::move(rhs.context);
        }
        return *this;
}

Diagnostics::Diagnostic &Diagnostics::Diagnostic::operator =(Diagnostics::Diagnostic &rhs)
{
        message = rhs.message;
        d       = rhs.d;
        d_type  = rhs.d_type;
        context = std::move(rhs.context);
        return *this;
}

Diagnostics::Diagnostic &Diagnostics::Diagnostic::operator ()()
{
        return *this;
}

std::ostream &Diagnostics::operator <<(std::ostream &os, const Diagnostics::Diagnostic &d_msg)
{
        os << d_msg.info << ' ' << diagnostic_colours[d_msg.type()] << ": " << d_msg.message << '\n';

        if (d_msg.has_context()) {
                for (auto &&context_item : d_msg.context) {
                        switch (context_item->type()) {
                        case Context::HIGHLIGHT:
                                Console::write_line(*(static_cast<HighlightContext *>(context_item.get())));
                                break;
                        case Context::SELECTOR:
                                Console::write_line(*(static_cast<SelectionContext *>(context_item.get())));
                                break;
                        default:
                                Console::write_line(*context_item);
                                break;
                        }
                }
        }

        return os;
}

void Diagnostics::reset()
{
        diagnostics_log.clear();
}
