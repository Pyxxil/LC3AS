#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

class String : public Token
{
public:
  String(std::string& string,
         std::string& t_file,
         size_t line_number,
         size_t t_column);

  virtual Token::token_type type() const override;
};

#endif // TOKEN_STRING_HPP
