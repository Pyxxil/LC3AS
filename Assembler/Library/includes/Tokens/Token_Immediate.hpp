#ifndef PROJECT_TOKEN_IMMEDIATE_HPP
#define PROJECT_TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token
{
public:
        Immediate();
        Immediate(std::string &word, std::string &token_uppercase, int line_number = 0);

        std::int16_t immediate = 0;

        virtual Token::token_type type() const override;
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_IMMEDIATE_HPP
