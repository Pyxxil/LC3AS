#ifndef PROJECT_TOKEN_OPERATOR_BR_HPP
#define PROJECT_TOKEN_OPERATOR_BR_HPP

#include "Token_Operator.hpp"

class Br : public Operator
{
public:
        Br(std::string &oper, int line_number = 0, bool n = false, bool z = false, bool p = false)
                : Operator(oper, line_number), N(n), Z(z), P(p)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_BR;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }

        std::uint16_t assemble() override
        {
                return static_cast<std::uint16_t >(0x0 | N << 10 | Z << 9 | P << 8);
        }

private:
        bool N;
        bool Z;
        bool P;
};

#endif //PROJECT_TOKEN_OPERATOR_BR_HPP
