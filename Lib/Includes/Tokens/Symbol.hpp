#ifndef PROJECT_SYMBOL_HPP
#define PROJECT_SYMBOL_HPP

#include <cstdint>
#include <cstdlib>

struct Symbol
{
        Symbol(std::uint16_t addr, int line)
                : address(addr), line_number(line)
        {

        }

        std::uint16_t address;
        int           line_number;
};

#endif //PROJECT_SYMBOL_HPP
