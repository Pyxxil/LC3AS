#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"

Decimal::Decimal(std::string &value, int line_number)
        : Immediate(value, value, line_number)
{
        if (value.length() > 7) {
                Token::expected("decimal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        if (value.at(0) == '#') {
                value.erase(0, 1);
        }

        try {
                std::size_t check = 0;
                immediate = static_cast<std::int16_t>(std::stoi(value, &check, 10));
                if (check != value.length()) {
                        is_valid = false;
                }
        } catch (const std::invalid_argument &e) {
                Token::expected("valid decimal value");
                is_valid = false;
        }

        if (!is_valid) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid literal for base 10: '" << value << "'.\n";
        }
}
