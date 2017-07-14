#ifndef TOKEN_IMMEDIATE_OCTAL_HPP
#define TOKEN_IMMEDIATE_OCTAL_HPP

#include <Tokens/Token_Immediate.hpp>

class Octal : public Immediate
{
public:
        Octal(std::string &immediate, std::string &t_file, int line_number);
};

#endif // TOKEN_IMMEDIATE_OCTAL_HPP
