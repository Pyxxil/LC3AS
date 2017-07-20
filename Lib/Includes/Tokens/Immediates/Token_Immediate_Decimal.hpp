#ifndef TOKEN_IMMEDIATE_DECIMAL_HPP
#define TOKEN_IMMEDIATE_DECIMAL_HPP

#include "Tokens/Token_Immediate.hpp"

class Decimal : public Immediate
{
public:
  Decimal(std::string& immediate,
          std::string& t_file,
          size_t line_number,
          size_t column);
  Decimal(std::string&& immediate,
          std::string& t_file,
          size_t line_number,
          size_t column)
    : Decimal(immediate, t_file, line_number, column)
  {}
};

#endif // TOKEN_IMMEDIATE_DECIMAL_HPP
