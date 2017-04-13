#ifndef PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "../Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &value, int line_number = 0);
};

#endif //PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
