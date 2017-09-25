#include "Diagnostics.hpp"

#include <algorithm>

#include "Configuration.hpp"

static std::deque<Diagnostics::Diagnostic> diagnostics_log;

const Diagnostics::diagnostic_type Diagnostics::diagnostic_colours[] = {
  Diagnostics::diagnostic_type("Warning",
                               Console::Colour(Console::FOREGROUND_COLOUR::CYAN,
                                               Console::MODIFIER::UNDERLINE)),
  Diagnostics::diagnostic_type("Error",
                               Console::Colour(Console::FOREGROUND_COLOUR::RED,
                                               Console::MODIFIER::UNDERLINE)),
  Diagnostics::diagnostic_type(
    "Note",
    Console::Colour(Console::FOREGROUND_COLOUR::BLACK, Console::MODIFIER::BOLD))
};

void
Diagnostics::unwind()
{
  while (!diagnostics_log.empty()) {
    auto&& diagnostic = diagnostics_log.front();
    Console::write(diagnostic);
    diagnostics_log.pop_front();
  }
}

void
Diagnostics::push(Diagnostic message)
{
  diagnostics_log.emplace_back(std::move(message));
}

bool
Diagnostics::critical()
{
  return std::any_of(diagnostics_log.cbegin(),
                     diagnostics_log.cend(),
                     [](auto&& diag) { return diag.is_critical(); });
}

size_t
Diagnostics::count()
{
  return diagnostics_log.size();
}

std::ostream&
Diagnostics::Diagnostic::write_to(std::ostream& os) const
{
  info.write_to(os) << ' ' << diagnostic_colours[type()] << ": " << message
                    << '\n';

  for (auto&& context_item : context) {
    Console::write_line(*context_item);
  }

  os << '\n';

  return os;
}

void
Diagnostics::reset()
{
  diagnostics_log.clear();
}
