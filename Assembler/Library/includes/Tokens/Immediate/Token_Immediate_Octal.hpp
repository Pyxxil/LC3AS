#ifndef TOKEN_IMMEDIATE_OCTAL_HPP
#define TOKEN_IMMEDIATE_OCTAL_HPP

#include <Tokens/Token_Immediate.hpp>

class Octal : public Immediate
{
public:
        Octal(std::string &immediate, int line_number = 0);
};

#endif // TOKEN_IMMEDIATE_OCTAL_HPP
