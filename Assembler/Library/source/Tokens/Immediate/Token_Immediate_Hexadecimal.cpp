#include "Tokens/Immediate/Token_Immediate_Hexadecimal.hpp"

Hexadecimal::Hexadecimal(std::string &immediate, std::string &immediate_uppercase, int line_number)
        : Immediate(immediate, immediate_uppercase, line_number)
{
        if (immediate.length() > 6 || immediate.length() == 1 ||
            (immediate.length() == 2 && std::toupper(immediate.at(1)) == 'X')) {
                Token::expected("hexadecimal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        if (std::toupper(immediate.at(0)) == 'X') {
                immediate.insert(0, 1, '0');
        }

        try {
                std::size_t check = 0;
                value = static_cast<std::int16_t>(std::stoi(immediate, &check, 16));
                if (check != immediate.length()) {
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

                std::cerr << "Invalid literal for base 16: '" << token << "'.\n";
        }
}
