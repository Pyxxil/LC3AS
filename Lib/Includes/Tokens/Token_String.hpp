#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

class String : public Token
{
public:
  String(const std::string& string,
         const std::string& t_file,
         size_t line_number,
         size_t t_column);

  inline token_type type() const final { return _STRING; }
};

#endif // TOKEN_STRING_HPP
