#ifndef TOKEN_DIRECTIVE_HPP
#define TOKEN_DIRECTIVE_HPP

#include "Token.hpp"

class Directive : public Token
{
public:
        Directive() = default;
        Directive(std::string &directive,
                  std::string &directive_uppercase,
                  std::string &t_file,
                  size_t line_number,
                  size_t column);
};

#endif // TOKEN_DIRECTIVE_HPP
