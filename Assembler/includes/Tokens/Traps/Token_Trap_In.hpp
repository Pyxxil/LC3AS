#ifndef PROJECT_TOKEN_TRAP_IN_HPP
#define PROJECT_TOKEN_TRAP_IN_HPP

#include "../Token_Instruction.hpp"

class In : public Instruction
{
public:
        In(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_IN;
        }
};


#endif //PROJECT_TOKEN_TRAP_IN_HPP
