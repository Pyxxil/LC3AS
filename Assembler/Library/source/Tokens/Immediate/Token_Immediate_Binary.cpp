#include "Tokens/Immediate/Token_Immediate_Binary.hpp"

Binary::Binary(std::string &value, int line_number)
        : Immediate(value, line_number)
{
        if (value.length() > 2 && std::toupper(value.at(1)) == 'B') {
                value.erase(0, 2);
        }  else if (std::toupper(value.at(0)) == 'B') {
                value.erase(0, 1);
        }

        if (value.length() > 16) {
                Token::expected("binary value up to 16 bits");
                return;
        }

        try {
                immediate = static_cast<std::int16_t >(std::bitset<16>(value).to_ulong());
        } catch (const std::invalid_argument &e) {
                Token::expected("valid binary value");
        }
}
