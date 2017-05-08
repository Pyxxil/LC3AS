#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"

Decimal::Decimal(std::string &immediate, std::string &t_file, int line_number)
        : Immediate(immediate, immediate, t_file, line_number)
{
        if (immediate.length() > 7) {
                Token::expected("decimal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        if (immediate.at(0) == '#') {
                immediate.erase(0, 1);
        }

        try {
                std::size_t check = 0;
                value = static_cast<std::int16_t>(std::stoi(immediate, &check, 10));
                if (check != immediate.length()) {
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

                std::cerr << "Invalid literal for base 10: '" << token << "'.\n";
        }
}
