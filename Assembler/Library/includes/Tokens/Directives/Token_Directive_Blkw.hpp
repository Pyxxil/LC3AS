#ifndef PROJECT_TOKEN_DIRECTIVE_BLKW_HPP
#define PROJECT_TOKEN_DIRECTIVE_BLKW_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Blkw : public Directive
{
public:
        Blkw(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual void invalid_argument_count(std::size_t provided, std::size_t expected) override;

private:
        bool first_time = true;
};

#endif //PROJECT_TOKEN_DIRECTIVE_BLKW_HPP
