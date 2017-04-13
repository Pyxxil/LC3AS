#ifndef PROJECT_TOKEN_DIRECTIVE_HPP
#define PROJECT_TOKEN_DIRECTIVE_HPP

#include "../Token.hpp"

class Directive : public Token
{
public:
        Directive() : Token()
        {}
        Directive(std::string &word, int line_number = 0) : Token(word, line_number)
        {}
};

#endif //PROJECT_TOKEN_DIRECTIVE_HPP
