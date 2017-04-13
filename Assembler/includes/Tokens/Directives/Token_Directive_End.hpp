#ifndef PROJECT_TOKEN_DIRECTIVE_END_HPP
#define PROJECT_TOKEN_DIRECTIVE_END_HPP

#include "../Token_Directive.hpp"

class End : public Directive
{
public:
        End(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_END_HPP
