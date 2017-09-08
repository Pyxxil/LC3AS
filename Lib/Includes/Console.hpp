#ifndef LC3_ASSEMBLER_CONSOLE_HPP
#define LC3_ASSEMBLER_CONSOLE_HPP

#include <iostream>
#include <sstream>
#include <string>

#if defined(_MSC_VER)
#define NOMINMAX
#include <Windows.h>
#undef ERROR
#endif

#include "Configuration.hpp"

namespace Console {
#if defined(_MSC_VER)
enum class FOREGROUND_COLOUR
{
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  YELLOW = 14,
  RED = 12,
  MAGENTA = 13,
  CYAN = 11,
  WHITE = 15,
  RESET = 7,
};

enum class BACKGROUND_COLOUR
{
  BLACK = 0x00,
  RED = 0x40,
  GREEN = 0x20,
  YELLOW = 0x60,
  BLUE = 0x10,
  MAGENTA = 0x50,
  CYAN = 0x30,
  WHITE = 0x70,
  RESET = BACKGROUND_COLOUR::BLACK,
};

enum class MODIFIER
{
  BOLD = FOREGROUND_INTENSITY,
  REVERSE = COMMON_LVB_REVERSE_VIDEO,
  UNDERLINE = COMMON_LVB_UNDERSCORE,
  NORMAL = 0,
  RESET = NORMAL,
};
#else
enum class FOREGROUND_COLOUR
{
  BLACK = 30,
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  MAGENTA = 35,
  CYAN = 36,
  WHITE = 37,
  RESET = 39
};

enum class BACKGROUND_COLOUR
{
  BLACK = 40,
  RED = 41,
  GREEN = 42,
  YELLOW = 43,
  BLUE = 44,
  MAGENTA = 45,
  CYAN = 46,
  WHITE = 47,
  RESET = 49
};

enum class MODIFIER
{
  RESET = 0,
  BOLD = 1,
  FAINT = 2,
  REVERSE = 3,
  UNDERLINE = 4,
  BLINK = 5,
  NORMAL = 22,
  FRAMED = 51,
  ENCIRCLED = 52,
  OVERLINED = 53,
};
#endif

struct Colour
{
  explicit Colour(FOREGROUND_COLOUR _fg,
                  MODIFIER modifier = MODIFIER::NORMAL,
                  BACKGROUND_COLOUR _bg = BACKGROUND_COLOUR::RESET)
    : fg(_fg)
    , bg(_bg)
    , mod(modifier)
  {}
  Colour(const Colour& other)
    : fg(other.fg)
    , bg(other.bg)
    , mod(other.mod)
  {}
  Colour(Colour&& other)
    : fg(std::move(other.fg))
    , bg(std::move(other.bg))
    , mod(std::move(other.mod))
  {}
  Colour& operator=(const Colour& other) = default;
  Colour& operator=(Colour&& other) noexcept
  {
    if (this != &other) {
      fg = other.fg;
      bg = other.bg;
      mod = other.mod;
    }

    return *this;
  }

  ~Colour() = default;

  friend std::ostream& operator<<(std::ostream& os, const Colour& colour)
  {
    if (!Config::is_set(Config::NO_COLOUR)) {
#if defined(_MSC_VER)
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              static_cast<int>(colour.fg) |
                                static_cast<int>(colour.bg) |
                                static_cast<int>(colour.mod));
#else
      return os << "\033[" << static_cast<int>(colour.fg) << ';'
                << static_cast<int>(colour.bg) << ';'
                << static_cast<int>(colour.mod) << 'm';
#endif
    }

    return os;
  }

  bool operator==(const Colour& other) const
  {
    return (fg == other.fg && bg == other.bg && mod == other.mod);
  }

  bool operator!=(const Colour& other) const { return !(*this == other); }

private:
  FOREGROUND_COLOUR fg;
  BACKGROUND_COLOUR bg;
  MODIFIER mod;
};

const Colour reset(FOREGROUND_COLOUR::RESET, MODIFIER::RESET);

template<typename T>
inline void
write(const T& message)
{
  message.write_to(std::cout);
}

template<typename T>
inline void
write(const T&& message)
{
  message.write_to(std::cout);
}

template<typename T>
void
write_line(const T& message)
{
  message.write_to(std::cout) << '\n';
}

template<typename T>
void
write_line(const T&& message)
{
  message.write_to(std::cout) << '\n';
}
}

#endif
