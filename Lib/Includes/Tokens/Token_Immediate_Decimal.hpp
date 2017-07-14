#ifndef TOKEN_IMMEDIATE_DECIMAL_HPP
#define TOKEN_IMMEDIATE_DECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Decimal : public Immediate
{
public:
        Decimal(std::string &immediate, std::string &t_file, int line_number = 0);
        Decimal(std::string &&immediate, std::string &t_file, int line_number = 0)
                : Decimal(immediate, t_file, line_number)
        { }
};

#endif // TOKEN_IMMEDIATE_DECIMAL_HPP
