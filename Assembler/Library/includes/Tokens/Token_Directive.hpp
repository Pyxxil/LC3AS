#ifndef PROJECT_TOKEN_DIRECTIVE_HPP
#define PROJECT_TOKEN_DIRECTIVE_HPP

#include "Token.hpp"

class Directive : public Token
{
public:
        Directive()
                : Token()
        {}
        Directive(std::string &token, std::string &token_as_uppercase, int line_number = 0)
                : Token(token, token_as_uppercase, line_number)
        {}
};

#endif //PROJECT_TOKEN_DIRECTIVE_HPP
