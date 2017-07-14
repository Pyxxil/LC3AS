#ifndef TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &immediate,
                    std::string &immediate_uppercase,
                    std::string &t_file,
                    int line_number);
};

#endif // TOKEN_IMMEDIATE_HEXADECIMAL_HPP
