#ifndef PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
#define PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP

#include "Token_Immediate.hpp"

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 6 || value.length() == 1) {
                        Token::expected("hexadecimal value up to 16 bits in length");
                        return;
                }

                if (value.length() == 2 && std::toupper(value.at(1)) == 'X') {
                        Token::expected("hexadecimal value up to 16 bits in length");
                        return;
                } else if (std::toupper(value.at(0)) == 'X') {
                        value.insert(0, 1, '0');
                }

                try {
                        std::size_t check = 0;
                        immediate = static_cast<std::uint16_t>(std::stoi(value, &check, 16));
                        if (check != value.length()) {
                                Token::expected("valid decimal value");
                        }
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid hexadecimal value");
                }
        }

        std::uint64_t requires() override
        { return static_cast<std::uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
