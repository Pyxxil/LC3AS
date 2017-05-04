#ifndef TOKEN_DIRECTIVE_HPP
#define TOKEN_DIRECTIVE_HPP

#include "Token.hpp"

class Directive : public Token
{
public:
        Directive();
        Directive(std::string &directive, std::string &directive_uppercase, int line_number = 0);
};

#endif // TOKEN_DIRECTIVE_HPP
