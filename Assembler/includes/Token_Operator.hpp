#ifndef PROJECT_TOKEN_OPERATOR_HPP
#define PROJECT_TOKEN_OPERATOR_HPP

#include "Token.hpp"

class Operator : public Token
{
public:
        Operator() : Token()
        {}
        Operator(std::string &oper, int line_number = 0) : Token(oper, line_number)
        {}
};

#endif //PROJECT_TOKEN_OPERATOR_HPP
