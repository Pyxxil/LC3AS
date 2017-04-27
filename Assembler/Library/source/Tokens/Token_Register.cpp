#include "Tokens/Token_Register.hpp"

Register::Register(std::string &which, std::string &which_uppercase, int line_number)
        : Token(which, which_uppercase, line_number), reg(which.at(1))
{
        // Valid registers are R0, R1, R2, R3, R4, R5, R6, and R7
        if (reg > 0x37 || reg < 0x30) {
                expected("register between 0 and 7");
        }
}

Token::token_type Register::type() const
{
        return Token::token_type::REGISTER;
}
