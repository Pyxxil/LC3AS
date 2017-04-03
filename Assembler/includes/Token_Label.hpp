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
        std::uint16_t address;

        virtual Token::token_type type() const override
        { return Token::token_type::LABEL; }

        std::uint64_t requires() override
        { return static_cast<std::uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_LABEL_HPP
