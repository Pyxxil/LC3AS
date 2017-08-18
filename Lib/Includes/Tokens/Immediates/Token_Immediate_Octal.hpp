#ifndef TOKEN_IMMEDIATE_OCTAL_HPP
#define TOKEN_IMMEDIATE_OCTAL_HPP

#include <Tokens/Token_Immediate.hpp>

class Octal : public Immediate
{
public:
  Octal(std::string& immediate,
        const std::string& t_file,
        size_t line_number,
        size_t t_column);
};

#endif // TOKEN_IMMEDIATE_OCTAL_HPP
