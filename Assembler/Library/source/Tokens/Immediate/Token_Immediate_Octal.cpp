#include "Tokens/Immediate/Token_Immediate_Octal.hpp"

Octal::Octal(std::string &value, int line_number)
        : Immediate(value, line_number)
{
        if (value.length() > 7 || value.length() == 1) {
                Token::expected("octal value up to 16 bits in length");
                return;
        }

        value.front() = '0';

        try {
                std::size_t check = 0;
                immediate = static_cast<std::int16_t>(std::stoi(value, &check, 8));
                if (check != value.length()) {
                        Token::expected("valid octal value");
                }
        } catch (const std::invalid_argument &e) {
                Token::expected("valid octal value");
        }
}
