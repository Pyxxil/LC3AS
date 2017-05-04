#ifndef TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &immediate, std::string &immediate_uppercase, int line_number = 0);
};

#endif // TOKEN_IMMEDIATE_HEXADECIMAL_HPP
