#ifndef PROJECT_TOKEN_DIRECTIVE_FILL_HPP
#define PROJECT_TOKEN_DIRECTIVE_FILL_HPP

#include "Tokens/Token_Directive.hpp"

class Fill : public Directive
{
public:
        // TODO: When actually implementing this, what about allowing .FILL LABEL, which
        // TODO: would fill the address with the address of LABEL?
        Fill(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_FILL_HPP
