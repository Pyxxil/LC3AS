#ifndef TOKEN_REGISTER_HPP
#define TOKEN_REGISTER_HPP

#include "Token.hpp"

class Register : public Token
{
public:
  Register(const std::string& which,
           const std::string& which_uppercase,
           const std::string& t_file,
           size_t line_number,
           size_t t_column);

  inline token_type type() const final { return REGISTER; }

  char reg;
};

#endif // TOKEN_REGISTER_HPP
