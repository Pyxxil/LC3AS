#ifndef PROJECT_TOKEN_LABEL_HPP
#define PROJECT_TOKEN_LABEL_HPP

#include "../Token.hpp"

class Label : public Token
{
        // TODO: Add some sort of fuzzy match to check if the user meant something else
public:
        Label(std::string &name, int line_number);

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual Token::token_type type() const override;
        void not_found();

        std::string label;

        std::uint16_t address = 0;
};

#endif //PROJECT_TOKEN_LABEL_HPP
