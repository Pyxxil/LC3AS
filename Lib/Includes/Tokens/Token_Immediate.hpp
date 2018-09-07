#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token {
public:
  Immediate() = default;
  Immediate(const std::string &immediate,
            const std::string &immediate_uppercase, const std::string &t_file,
            size_t line_number, size_t t_column);

  inline virtual token_type type() const final { return IMMEDIATE; }

  void
  requires_too_many_bits(int allowed_bits, Signedness is_signed,
                         const Token *caller,
                         const std::map<std::string, Symbol> &symbols) final;

  std::int16_t value = 0;
};

#endif // TOKEN_IMMEDIATE_HPP
