#ifndef PROJECT_TOKEN_OPERATOR_BR_HPP
#define PROJECT_TOKEN_OPERATOR_BR_HPP

#include "../Token_Instruction.hpp"

class Br : public Instruction
{
public:
        Br(std::string &oper, int line_number = 0, bool n = false, bool z = false, bool p = false);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;

        std::vector<uint16_t> as_assembled() override;

private:
        bool N;
        bool Z;
        bool P;
};

#endif //PROJECT_TOKEN_OPERATOR_BR_HPP
