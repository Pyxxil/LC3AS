#ifndef PROJECT_SYMBOL_HPP
#define PROJECT_SYMBOL_HPP

#include <cstdint>
#include <cstdlib>

struct Symbol {
    Symbol(uint16_t addr, size_t line, size_t t_column,
           const std::string &t_file)
        : address(addr), line_number(line), column(t_column), file(t_file)
    {
    }

    uint16_t address;
    size_t line_number;
    size_t column;
    std::string file;
};

#endif // PROJECT_SYMBOL_HPP
