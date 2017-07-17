#ifndef TOKEN_INSTRUCTION_HPP
#define TOKEN_INSTRUCTION_HPP

#include "Token.hpp"

class Instruction: public Token
{
public:
    Instruction() = default;
    Instruction(std::string &instruction,
                std::string &instruction_uppercase,
                std::string &t_file,
                size_t line_number,
                size_t column);
};

#endif // TOKEN_INSTRUCTION_HPP
