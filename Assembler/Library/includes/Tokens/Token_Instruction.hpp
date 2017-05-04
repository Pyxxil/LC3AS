#ifndef TOKEN_INSTRUCTION_HPP
#define TOKEN_INSTRUCTION_HPP

#include "Token.hpp"

class Instruction : public Token
{
public:
        Instruction();
        Instruction(std::string &instruction, std::string &instruction_uppercase, int line_number = 0);
};

#endif // TOKEN_INSTRUCTION_HPP
