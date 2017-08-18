#ifndef TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
  Hexadecimal(std::string& immediate,
              const std::string& immediate_uppercase,
              const std::string& t_file,
              size_t line_number,
              size_t t_column);
};

#endif // TOKEN_IMMEDIATE_HEXADECIMAL_HPP
