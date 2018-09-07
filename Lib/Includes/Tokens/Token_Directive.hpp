#ifndef TOKEN_DIRECTIVE_HPP
#define TOKEN_DIRECTIVE_HPP

#include "Token.hpp"

class Directive : public Token {
public:
  Directive() = default;
  Directive(const std::string &directive,
            const std::string &directive_uppercase, const std::string &t_file,
            size_t line_number, size_t t_column);
};

#endif // TOKEN_DIRECTIVE_HPP
