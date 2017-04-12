#ifndef PROJECT_TOKEN_OPERATOR_LEA_HPP
#define PROJECT_TOKEN_OPERATOR_LEA_HPP

#include "Token_Operator.hpp"

class Lea : public Operator
{
public:
        Lea(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_LEA;
        }

        int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 3) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("LEA after .END directive. It will be ignored");
                        return 0;
                }

                if (tokens[1]->type() != Token::REGISTER) {
                        tokens[1]->expected("register");
                        return -1;
                } else if (tokens[2]->type() != Token::LABEL) {
                        tokens[2]->expected("label");
                        return -1;
                }

                assembled.push_back(static_cast<std::uint16_t >(0xE000 |
                        ((static_cast<Register *>(tokens[1])->reg & 7) << 9)));

                return 1;
        }

        std::vector<uint16_t> as_assembled() override
        {
                return Token::as_assembled();
        }
};

#endif //PROJECT_TOKEN_OPERATOR_LEA_HPP
