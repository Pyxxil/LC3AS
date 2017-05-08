#ifndef TOKEN_INSTRUCTION_HPP
#define TOKEN_INSTRUCTION_HPP

#include "Token.hpp"

class Instruction : public Token
{
public:
        Instruction();
        Instruction(std::string &instruction,
                    std::string &instruction_uppercase,
                    std::string &t_file,
                    int line_number);
};

#endif // TOKEN_INSTRUCTION_HPP
