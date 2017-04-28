#ifndef PROJECT_TOKEN_DIRECTIVE_HPP
#define PROJECT_TOKEN_DIRECTIVE_HPP

#include "Token.hpp"

class Directive : public Token
{
public:
        Directive();
        Directive(std::string &directive, std::string &directive_uppercase, int line_number = 0);
};

#endif //PROJECT_TOKEN_DIRECTIVE_HPP
