#include "Tokens/Immediate/Token_Immediate_Hexadecimal.hpp"

Hexadecimal::Hexadecimal(std::string &value, std::string &value_uppercase, int line_number)
        : Immediate(value, value_uppercase, line_number)
{
        if (value.length() > 6 || value.length() == 1 || (value.length() == 2 && std::toupper(value.at(1)) == 'X')) {
                Token::expected("hexadecimal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        if (std::toupper(value.at(0)) == 'X') {
                value.insert(0, 1, '0');
        }

        try {
                std::size_t check = 0;
                immediate = static_cast<std::int16_t>(std::stoi(value, &check, 16));
                if (check != value.length()) {
                        Token::expected("valid hexadecimal value");
                        is_valid = false;
                }
        } catch (const std::invalid_argument &e) {
                Token::expected("valid hexadecimal value");
                is_valid = false;
        }

        if (!is_valid) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid literal for base 16: '" << value << "'.\n";
        }
}
