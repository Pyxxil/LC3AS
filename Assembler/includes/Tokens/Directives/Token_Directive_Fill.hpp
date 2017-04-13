#ifndef PROJECT_TOKEN_DIRECTIVE_FILL_HPP
#define PROJECT_TOKEN_DIRECTIVE_FILL_HPP

#include "../Token_Directive.hpp"

class Fill : public Directive
{
public:
        // TODO: When actually implementing this, what about allowing .FILL LABEL, which
        // TODO: would fill the address with the address of LABEL?
        Fill(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_FILL_HPP
