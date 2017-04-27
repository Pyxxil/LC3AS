#include "Tokens/Immediate/Token_Immediate_Octal.hpp"

Octal::Octal(std::string &value, int line_number)
        : Immediate(value, value, line_number)
{
        if (value.length() > 7 || value.length() == 1) {
                Token::expected("octal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        value.front() = '0';

        try {
                std::size_t check = 0;
                immediate = static_cast<std::int16_t>(std::stoi(value, &check, 8));
                if (check != value.length()) {
                        Token::expected("valid octal value");
                        is_valid = false;
                }
        } catch (const std::invalid_argument &e) {
                Token::expected("valid octal value");
                is_valid = false;
        }

        if (!is_valid) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid literal for base 8: '" << value << "'.\n";
        }
}
