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

                if (std::toupper(value.at(1)) == 'X' && value.length() == 2) {
                        Token::expected("hexadecimal value up to 16 bits in length");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t>(std::strtoul(value.c_str(), nullptr, 16));
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid hexadecimal value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_IMMEDIATE_HEXADECIMAL_HPP
