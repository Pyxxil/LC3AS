#include "Tokens/Token_Immediate_Binary.hpp"

Binary::Binary(std::string &immediate, std::string &immediate_uppercase, std::string &t_file, int line_number)
        : Immediate(immediate, immediate_uppercase, t_file, line_number)
{
        bool negative = false;

        if (immediate.at(0) == '-') {
                negative = true;
                immediate.erase(0, 1);
        }

        if (immediate.length() > 2 && std::toupper(immediate.at(1)) == 'B') {
                immediate.erase(0, 2);
        } else if (std::toupper(immediate.at(0)) == 'B') {
                immediate.erase(0, 1);
        }

        if (immediate.length() > 16) {
                Token::expected("binary value up to 16 bits");
                is_valid = false;
                return;
        }

        try {
                value = static_cast<std::int16_t>(std::bitset<16>(immediate).to_ulong());
        } catch (const std::invalid_argument &e) {
                Token::expected("valid binary value");
                is_valid = false;
        }

        if (!is_valid) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid literal for base 2: '" << immediate << "'.\n";
        } else {
                if (negative) {
                        value = static_cast<std::int16_t>(-value);
                }
        }
}
