#ifndef PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &immediate, std::string &immediate_uppercase, int line_number = 0);
};

#endif //PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
