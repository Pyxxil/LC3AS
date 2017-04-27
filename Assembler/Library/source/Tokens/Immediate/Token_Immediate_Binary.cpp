#include "Tokens/Immediate/Token_Immediate_Binary.hpp"

Binary::Binary(std::string &value, std::string &value_uppercase, int line_number)
        : Immediate(value, value_uppercase, line_number)
{
        std::int16_t sign = 1;

        if (value.at(0) == '-') {
                sign = -1;
                value.erase(0, 1);
        }

        if (value.length() > 2 && std::toupper(value.at(1)) == 'B') {
                value.erase(0, 2);
        } else if (std::toupper(value.at(0)) == 'B') {
                value.erase(0, 1);
        }

        if (value.length() > 16) {
                Token::expected("binary value up to 16 bits");
                is_valid = false;
                return;
        }

        try {
                immediate = static_cast<std::int16_t>(std::bitset<16>(value).to_ulong());
        } catch (const std::invalid_argument &e) {
                Token::expected("valid binary value");
                is_valid = false;
        }

        if (!is_valid) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid literal for base 2: '" << value << "'.\n";
        } else {
                immediate *= sign;
        }
}
