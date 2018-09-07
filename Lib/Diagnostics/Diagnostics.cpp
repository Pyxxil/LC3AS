#include "Diagnostics.hpp"

#include <algorithm>
#include <fmt/ostream.h>

#include "Configuration.hpp"

static std::deque<Diagnostics::Diagnostic> diagnostics_log;
static bool is_critical = false;

const Diagnostics::diagnostic_type Diagnostics::diagnostic_colours[] = {
    Diagnostics::diagnostic_type(
        "Warning", Console::Colour(Console::FOREGROUND_COLOUR::CYAN,
                                   Console::MODIFIER::UNDERLINE)),
    Diagnostics::diagnostic_type(
        "Error", Console::Colour(Console::FOREGROUND_COLOUR::RED,
                                 Console::MODIFIER::UNDERLINE)),
    Diagnostics::diagnostic_type(
        "Note", Console::Colour(Console::FOREGROUND_COLOUR::BLACK,
                                Console::MODIFIER::BOLD))};

void Diagnostics::unwind() {
  std::for_each(diagnostics_log.begin(), diagnostics_log.end(),
                [](auto &&diagnositc) { fmt::print("{}", diagnositc); });
  diagnostics_log.clear();
}

void Diagnostics::push(Diagnostic message) {
  if (ERROR == message.type()) {
    is_critical = true;
  }
  diagnostics_log.emplace_back(std::move(message));
}

bool Diagnostics::critical() { return is_critical; }

size_t Diagnostics::count() { return diagnostics_log.size(); }

std::ostream &Diagnostics::Diagnostic::write_to(std::ostream &os) const {
  info.write_to(os) << ' ' << diagnostic_colours[type()] << ": " << message
                    << '\n';

  for (auto &&context_item : context) {
    os << *context_item << '\n';
  }

  return os;
}

void Diagnostics::reset() {
  diagnostics_log.clear();
  is_critical = false;
}
