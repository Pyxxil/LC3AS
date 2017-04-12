#ifndef PROJECT_TOKEN_TRAP_OUT_HPP
#define PROJECT_TOKEN_TRAP_OUT_HPP

#include "Token_Operator.hpp"

class Out : public Operator
{
public:
        Out(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_OUT;
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
                        WARNING("OUT after .END directive. It will be ignored");
                        return 0;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_TRAP_OUT_HPP
