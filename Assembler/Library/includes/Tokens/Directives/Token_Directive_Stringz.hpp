#ifndef PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP
#define PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP

#include "Tokens/Token_Directive.hpp"

class Stringz : public Directive
{
public:
        Stringz(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP
