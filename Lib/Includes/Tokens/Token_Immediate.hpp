#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token
{
public:
  Immediate() = default;
  Immediate(std::string& immediate,
            std::string& immediate_uppercase,
            std::string& t_file,
            size_t line_number,
            size_t column);

  virtual Token::token_type type() const override;
  void requires_too_many_bits(
    int allowed_bits,
    bool is_signed,
    const Token* caller,
    const std::map<std::string, Symbol>& symbols) override;

  std::int16_t value = 0;
};

#endif // TOKEN_IMMEDIATE_HPP
