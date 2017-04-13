#ifndef PROJECT_TOKEN_OPERATOR_JSR_HPP
#define PROJECT_TOKEN_OPERATOR_JSR_HPP

#include "../Token_Instruction.hpp"

class Jsr : public Instruction
{
public:
        Jsr(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;

        std::vector<uint16_t, std::allocator<uint16_t>> as_assembled() override
        {
                return Token::as_assembled();
        }
};

#endif //PROJECT_TOKEN_OPERATOR_JSR_HPP
