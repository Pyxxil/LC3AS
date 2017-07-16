#ifndef LC3_ASSEMBLER_CONSOLE_HPP
#define LC3_ASSEMBLER_CONSOLE_HPP

#include <iostream>
#include <sstream>
#include <string>

// TODO: Make this cross-platform
//#ifdef _WIN32
//#include <windows.h>
//#endif

namespace Console
{
        enum class FOREGROUND_COLOUR
        {
                BLACK   = 30,
                RED     = 31,
                GREEN   = 32,
                YELLOW  = 33,
                BLUE    = 34,
                MAGENTA = 35,
                CYAN    = 36,
                WHITE   = 37,
                RESET   = 39
        };

        enum class BACKGROUND_COLOUR
        {
                BLACK   = 40,
                RED     = 41,
                GREEN   = 42,
                YELLOW  = 43,
                BLUE    = 44,
                MAGENTA = 45,
                CYAN    = 46,
                WHITE   = 47,
                RESET   = 49
        };

        enum class MODIFIER
        {
                RESET     = 0,
                BOLD      = 1,
                FAINT     = 2,
                REVERSE   = 3,
                UNDERLINE = 4,
                BLINK     = 5,
                NORMAL    = 22,
                FRAMED    = 51,
                ENCIRCLED = 52,
                OVERLINED = 53,
        };

        struct Colour
        {
                explicit Colour(FOREGROUND_COLOUR _fg,
                       MODIFIER modifier = MODIFIER::NORMAL,
                       BACKGROUND_COLOUR _bg = BACKGROUND_COLOUR::RESET)
                        : fg(_fg), bg(_bg), mod(modifier)
                { }
                Colour(const Colour &other) = default;
                Colour(Colour &&other) = default;
                Colour &operator =(const Colour &other) = default;
                Colour &operator =(Colour &&other) noexcept
                {
                        if (this != &other) {
                                fg  = other.fg;
                                bg  = other.bg;
                                mod = other.mod;
                        }

                        return *this;
                }

                ~Colour() = default;

                friend std::ostream &operator <<(std::ostream &os, const Colour &colour)
                {
                        return os << "\033[" << static_cast<int>(colour.fg) << ';' << static_cast<int>(colour.bg)
                                  << ';' << static_cast<int>(colour.mod) << 'm';
                }

                bool operator ==(const Colour &other) const
                {
                        return (fg == other.fg && bg == other.bg && mod == other.mod);
                }

                bool operator !=(const Colour &other) const
                {
                        return !(*this == other);
                }

        private:
                FOREGROUND_COLOUR fg;
                BACKGROUND_COLOUR bg;
                MODIFIER          mod;
        };

        extern const Colour reset;

        template <typename T>
        inline void write(const T &message)
        {
                message.write_to(std::cout);
        }

        template <typename T>
        inline void write(const T &&message)
        {
                message.write_to(std::cout);
        }

        template <typename T>
        void write_line(const T &message)
        {
                write(message);
                std::cout << '\n';
        }

        template <typename T>
        void write_line(const T &&message)
        {
                write(message);
                std::cout << '\n';
        }
}

#endif
