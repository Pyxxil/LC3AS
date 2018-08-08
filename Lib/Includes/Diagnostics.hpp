#ifndef LC3_ASSEMBLER_DIAGNOSTICS_HPP
#define LC3_ASSEMBLER_DIAGNOSTICS_HPP

#include <memory>
#include <queue>
#include <string>
#include <utility>

#include "Context.hpp"

namespace Diagnostics {
enum DIAGNOSTIC {
  WARNING = 0,
  ERROR = 1,
  NOTE = 2,
};

enum DIAGNOSTIC_TYPE {
  SYNTAX,
  SPELLING,
  INVALID_LABEL,
  MULTIPLE_DEFINITIONS,
  INVALID_LITERAL,
  LOGIC,
  INVALID_FILE,
};

struct diagnostic_type {
  diagnostic_type(std::string t, Console::Colour col)
      : type(std::move(t)), colour(std::move(col)) {}

  friend std::ostream &operator<<(std::ostream &os, const diagnostic_type &t) {
    return os << t.colour << t.type << Console::reset;
  }

  std::string type;
  Console::Colour colour;
};

extern const diagnostic_type diagnostic_colours[];

class Diagnostic {
public:
  Diagnostic(FileContext file, std::string t_message, DIAGNOSTIC_TYPE t_type,
             DIAGNOSTIC diagnostic)
      : message(std::move(t_message)), d_type(t_type),
        d(Config::is_set(Config::WARN_AS_ERROR) ? ERROR : diagnostic),
        context(), info(std::move(file)) {}

  Diagnostic(Diagnostic &other)
      : message(other.message), d_type(other.d_type), d(other.d),
        context(std::move(other.context)), info(other.info) {}
  Diagnostic(Diagnostic &&other) = default;

  Diagnostic &operator=(const Diagnostic &rhs) = default;
  Diagnostic &operator=(Diagnostic &&rhs) = default;

  ~Diagnostic() = default;

  inline void provide_context(std::unique_ptr<Context> &&t_context) {
    context.emplace_back(std::move(t_context));
  }

  inline DIAGNOSTIC type() const { return d; }

  inline bool has_context() const { return !context.empty(); }

  inline bool is_critical() const { return type() == Diagnostics::ERROR; }

  std::ostream &write_to(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const Diagnostic &diag) {
    diag.write_to(os);
    return os << '\n';
  }

private:
  std::string message;
  DIAGNOSTIC_TYPE d_type;

  DIAGNOSTIC d;

  std::vector<std::unique_ptr<Context>> context;
  FileContext info;
};

void push(Diagnostic message);

size_t count();

void unwind();

bool critical();

void reset();
} // namespace Diagnostics

#endif
