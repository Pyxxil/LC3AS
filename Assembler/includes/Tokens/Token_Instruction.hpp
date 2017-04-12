#ifndef PROJECT_TOKEN_INSTRUCTION_HPP
#define PROJECT_TOKEN_INSTRUCTION_HPP

#include "../Token.hpp"

class Instruction : public Token
{
public:
        Instruction() : Token()
        {}
        Instruction(std::string &oper, int line_number = 0) : Token(oper, line_number)
        {}
};

#endif // PROJECT_TOKEN_INSTRUCTION_HPP
