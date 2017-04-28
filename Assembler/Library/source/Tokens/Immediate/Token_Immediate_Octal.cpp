#include "Tokens/Immediate/Token_Immediate_Octal.hpp"

Octal::Octal(std::string &immediate, int line_number)
        : Immediate(immediate, immediate, line_number)
{
        if (immediate.length() > 7 || immediate.length() == 1) {
                Token::expected("octal value up to 16 bits in length");
                is_valid = false;
                return;
        }

        immediate.front() = '0';

        try {
                std::size_t check = 0;
                value = static_cast<std::int16_t>(std::stoi(immediate, &check, 8));
                if (check != immediate.length()) {
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
                std::cerr << "Invalid literal for base 8: '" << immediate << "'.\n";
        }
}
