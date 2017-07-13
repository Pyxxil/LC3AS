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

        struct Colour
        {
                Colour(FOREGROUND_COLOUR _fg, BACKGROUND_COLOUR _bg = BACKGROUND_COLOUR::RESET)
                        : fg(_fg), bg(_bg)
                { }
                Colour(const Colour &other)
                        : fg(other.fg), bg(other.bg)
                { }
                Colour &operator =(const Colour &other)
                {
                        fg = other.fg;
                        bg = other.bg;
                        return *this;
                }

                FOREGROUND_COLOUR fg;

                BACKGROUND_COLOUR bg;
                friend std::ostream &operator <<(std::ostream &os, const Colour &colour)
                {
                        return os << "\033[" << static_cast<int>(colour.fg) << ';' << static_cast<int>(colour.bg)
                                  << 'm';
                }

                bool operator ==(const Colour &other) const
                {
                        return (fg == other.fg && bg == other.bg);
                }

                bool operator !=(const Colour &other) const
                {
                        return !(*this == other);
                }
        };

        static const Colour reset(FOREGROUND_COLOUR::RESET, BACKGROUND_COLOUR::RESET);

        template<typename T>
        inline void write(const T &message)
        {
                std::cout << message;
        }

        template<typename T>
        inline void write(const T &&message)
        {
                std::cout << message;
        }

        template<typename T>
        void write_line(const T &message)
        {
                write(message);
                std::cout << '\n';
        }

        template<typename T>
        void write_line(const T &&message)
        {
                write(message);
                std::cout << '\n';
        }
}

#endif
