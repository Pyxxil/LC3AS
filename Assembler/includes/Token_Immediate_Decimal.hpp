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
                                value.erase(0, 1);
                        }

                        std::size_t check = 0;
                        immediate = static_cast<std::int16_t>(std::stoi(value, &check, 10));
                        if (check != value.length()) {
                                Token::expected("valid decimal value");
                        }
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid decimal value");
                }
        }
};

#endif //PROJECT_TOKEN_IMMEDIATE_DECIMAL_HPP
