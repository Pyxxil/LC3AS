#include "Context.hpp"

#include <iomanip>
#if defined(_MSC_VER)
#include <cctype>
#endif

#include "Diagnostics.hpp"

static const Console::Colour HIGHLIGHTER(Console::FOREGROUND_COLOUR::MAGENTA);

std::ostream&
Diagnostics::Context::write_to(std::ostream& os) const
{
  return file_information.write_to(os)
         << ' ' << diagnostic_colours[DIAGNOSTIC::NOTE] << ": "
         << Console::reset << message;
}

Diagnostics::HighlightContext::HighlightContext(SelectionContext t_selector,
                                                char t_highlighter,
                                                int t_highlight_length,
                                                std::string changer)
  : Context(std::move(t_selector.file_information),
            t_selector.message,
            t_selector.line,
            HIGHLIGHT)
  , highlighter(t_highlighter)
  , highlight_length(t_highlight_length - 1)
  , selector(t_selector)
  , fix_it(std::move(changer))
{}

std::ostream&
Diagnostics::HighlightContext::write_to(std::ostream& os) const
{
  selector.write_to(os);

  if (0 != highlight_length) {
    // For the cases when we're attempting to highlight something
    // with a single character, which is already highlighted by the
    // selection context, and so we won't need the extra highlight
    // character that this will produce.
    os << HIGHLIGHTER << std::setw(highlight_length)
       << std::setfill(highlighter) << '~';
  }

  if (!fix_it.empty()) {
    os << '\n' << empty_line << fix_it;
  }

  return os << Console::reset;
}

Diagnostics::SelectionContext::SelectionContext(Diagnostics::FileContext file,
                                                char t_selector,
                                                std::string t_message,
                                                const std::string& t_line,
                                                std::string changer)
  : Context(std::move(file), std::move(t_message), t_line, SELECTOR)
  , selector(t_selector)
  , fix_it(std::move(changer))
{}

std::ostream&
Diagnostics::SelectionContext::write_to(std::ostream& os) const
{
  file_information.write_to(os)
    << ' ' << diagnostic_colours[DIAGNOSTIC::NOTE] << ": " << message << '\n'
    << line << '\n'
    << HIGHLIGHTER << empty_line << selector;

  if (!fix_it.empty()) {
    os << '\n' << empty_line << fix_it;
  }

  return os << Console::reset;
}
