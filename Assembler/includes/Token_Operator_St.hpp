#ifndef PROJECT_TOKEN_OPERATOR_ST_HPP
#define PROJECT_TOKEN_OPERATOR_ST_HPP

#include "Token_Operator.hpp"

class St : public Operator
{
public:
        St(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_ST;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 3) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("PUTS after .END directive. It will be ignored");
                        return 0;
                }

                if (tokens[1]->type() != Token::REGISTER) {
                        tokens[1]->expected("register");
                        return -1;
                } else if (tokens[2]->type() != Token::LABEL) {
                        tokens[2]->expected("label");
                        return -1;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_ST_HPP
