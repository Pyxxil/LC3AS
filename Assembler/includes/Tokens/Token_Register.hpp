#ifndef PROJECT_TOKEN_REGISTER_HPP
#define PROJECT_TOKEN_REGISTER_HPP

#include "../Token.hpp"

class Register : public Token
{
public:
        Register(std::string which, int line_number = 0) : Token(which, line_number), reg(which.at(1))
        {
                // Valid registers are R0, R1, R2, R3, R4, R5, R6, and R7
                if (reg > 0x37 || reg < 0x30) {
                        expected("register between 0 and 7");
                }
        }

        char reg;

        virtual Token::token_type type() const override
        { return Token::token_type::REGISTER; }
};

#endif //PROJECT_TOKEN_REGISTER_HPP
