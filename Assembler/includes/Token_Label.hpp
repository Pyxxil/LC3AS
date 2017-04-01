#ifndef PROJECT_TOKEN_LABEL_HPP
#define PROJECT_TOKEN_LABEL_HPP

#include "Token.hpp"

class Label : public Token
{

        // TODO: Add some sort of fuzzy match to check if the user meant something else
public:
        Label(std::string &name, int line_number) : Token(name, line_number), label(name)
        {}

        std::string label;

        Token::token_type type() override
        { return Token::token_type::LABEL; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_LABEL_HPP
