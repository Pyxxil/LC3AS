#ifndef PROJECT_TOKEN_OPERATOR_RET_HPP
#define PROJECT_TOKEN_OPERATOR_RET_HPP

#include "Token_Operator.hpp"

class Ret : public Operator
{
public:
        Ret(std::string &word, int line_number = 0) : Operator(word, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_RET;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 1) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("RET after .END directive. It will be ignored");
                        return 0;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_RET_HPP
