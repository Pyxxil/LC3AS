#ifndef PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
#define PROJECT_TOKEN_IMMEDIATE_BINARY_HPP

#include "../Token_Immediate.hpp"

#include <bitset>

class Binary : public Immediate
{
public:
        Binary(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 16) {
                        Token::expected("binary value up to 16 bits");
                        return;
                } else if (value.length() > 2 && std::toupper(value.at(1)) == 'B') {
                        value.erase(0, 2);
                } else if (std::toupper(value.at(0)) == 'B') {
                        value.erase(0, 1);
                }

                try {
                        immediate = static_cast<std::int16_t >(std::bitset<16>(value).to_ulong());
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid binary value");
                }
        }
};

#endif // PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
