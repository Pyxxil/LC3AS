#include "Context.hpp"

#include <iomanip>

#include "Diagnostics.hpp"

static const Console::Colour HIGHLIGHTER(Console::FOREGROUND_COLOUR::MAGENTA);

Diagnostics::FileContext::FileContext(std::string &name, size_t t_line, size_t t_col)
        : file_name(name), column(t_col), line(t_line)
{ }

Diagnostics::FileContext::FileContext(std::string &&name, size_t t_line, size_t t_col)
        : file_name(name), column(t_col), line(t_line)
{ }

Diagnostics::FileContext::FileContext(Diagnostics::Variant<std::string> &name,
                                      Diagnostics::Variant<size_t> &t_line,
                                      Diagnostics::Variant<size_t> &t_col)
        : file_name(name), column(t_col), line(t_line)
{ }

Diagnostics::FileContext::FileContext(Diagnostics::Variant<std::string> &&name,
                                      Diagnostics::Variant<size_t> &&t_line,
                                      Diagnostics::Variant<size_t> &&t_col)
        : file_name(name), column(t_col), line(t_line)
{ }

std::ostream &Diagnostics::operator <<(std::ostream &os, const Diagnostics::FileContext &file)
{
        return os << "In " << file.file_name << ':' << file.line << ':' << file.column << ':';
}

Diagnostics::Context::Context(const Diagnostics::FileContext &file,
                              const std::string &t_message,
                              const std::string &t_line,
                              Context::CONTEXT_TYPE t_type)
        : file_information(file), message(t_message), line(t_line), context_type(t_type)
{
        for (size_t i = 0; i < file.get_column().var(); ++i) {
                empty_line += std::isspace(t_line[i]) ? t_line[i] : ' ';
        }
}

Diagnostics::Context::Context(const Diagnostics::Context &other)
        : file_information(other.file_information), message(other.message), line(other.line)
          , empty_line(other.empty_line), context_type(other.context_type)
{ }

Diagnostics::Context::Context(const Diagnostics::Context &&other)
        : file_information(other.file_information), message(other.message), line(other.line)
          , empty_line(other.empty_line), context_type(other.context_type)
{

}

Diagnostics::Context &Diagnostics::Context::operator =(const Diagnostics::Context &&rhs)
{
        if (this != &rhs) {
                file_information = rhs.file_information;
                message          = rhs.message;
                line             = rhs.line;
                context_type     = rhs.context_type;
                empty_line       = rhs.empty_line;
        }

        return *this;
}

Diagnostics::Context &Diagnostics::Context::operator =(const Diagnostics::Context &rhs)
{
        file_information = rhs.file_information;
        message          = rhs.message;
        line             = rhs.line;
        context_type     = rhs.context_type;
        return *this;
}

Diagnostics::Context::Context(const Diagnostics::FileContext &file,
                              const std::string &&t_message,
                              const std::string &&t_line,
                              Diagnostics::Context::CONTEXT_TYPE t_type)
        : file_information(file), message(t_message), line(t_line), empty_line(), context_type(t_type)
{
        for (size_t i = 0; i < file.get_column().var(); ++i) {
                empty_line += std::isspace(t_line[i]) ? t_line[i] : ' ';
        }
}

std::ostream &Diagnostics::operator <<(std::ostream &os, const Diagnostics::Context &con)
{
        return os << con.file_information << ' ' << diagnostic_colours[DIAGNOSTIC::NOTE] << ": "
                  << Console::reset << con.message;
}

Diagnostics::HighlightContext::HighlightContext(const SelectionContext &t_selector,
                                                char t_highlighter,
                                                int t_highlight_length,
                                                const std::string &changer)
        : Context(t_selector.file_information, t_selector.message, t_selector.line, HIGHLIGHT)
          , highlighter(t_highlighter), highlight_length(t_highlight_length - 1), selector(t_selector)
          , fix_it(changer)
{ }

std::ostream &Diagnostics::operator <<(std::ostream &os, const Diagnostics::HighlightContext &context)
{

        os << context.selector;

        if (context.highlight_length) {
                // For the cases when we're attempting to highlight something with a single character,
                // which is already highlighted by the selection context, and so we won't need the extra
                // highlight character that this will produce.
                os << HIGHLIGHTER << std::setw(context.highlight_length)
                   << std::setfill(context.highlighter) << '~';
        }

        if (!context.fix_it.empty()) {
                os << '\n' << context.empty_line << context.fix_it;
        }

        return os << Console::reset;
}

Diagnostics::SelectionContext::SelectionContext(const Diagnostics::FileContext &file,
                                                char t_selector,
                                                const std::string &t_message,
                                                const std::string &t_line,
                                                const std::string &changer)
        : Context(file, t_message, t_line, SELECTOR), selector(t_selector), fix_it(changer)
{ }

Diagnostics::SelectionContext::SelectionContext(const Diagnostics::FileContext &file,
                                                char t_selector,
                                                const std::string &&t_message,
                                                const std::string &&t_line,
                                                const std::string &changer)
        : Context(file, t_message, t_line, SELECTOR), selector(t_selector), fix_it(changer)
{ }

std::ostream &Diagnostics::operator <<(std::ostream &os, const Diagnostics::SelectionContext &context)
{
        os << context.file_information << ' ' << diagnostic_colours[DIAGNOSTIC::NOTE] << ": "
           << context.message << '\n' << context.line << '\n' << HIGHLIGHTER << context.empty_line
           << context.selector;

        if (!context.fix_it.empty()) {
                os << '\n' << context.empty_line << context.fix_it;
        }

        return os << Console::reset;
}
