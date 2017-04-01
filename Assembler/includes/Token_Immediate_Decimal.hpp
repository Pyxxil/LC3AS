#ifndef PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP
#define PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP

#include "Token_Immediate.hpp"

class Decimal : public Immediate
{
public:
        Decimal(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 7) {
                        Token::expected("decimal value up to 16 bits in length");
                        return;
                }

                try {
                        if (value.at(0) == '#') {
                                value.erase(0);
                        }
                        immediate = static_cast<uint16_t>(std::strtol(value.c_str(), nullptr, 10));
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid decimal value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP
