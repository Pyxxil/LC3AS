#ifndef PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP
#define PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Decimal : public Immediate
{
public:
        Decimal(std::string &immediate, int line_number = 0);
        Decimal(std::string &&immediate, int line_number = 0)
                : Decimal(immediate, line_number)
        {}
};

#endif //PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP
