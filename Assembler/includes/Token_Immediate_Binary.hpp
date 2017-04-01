#ifndef PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
#define PROJECT_TOKEN_IMMEDIATE_BINARY_HPP

#include "Token_Immediate.hpp"

class Binary : public Immediate
{
public:
        Binary(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 16) {
                        Token::expected("binary value up to 16 bits");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t >(std::bitset<16>(value).to_ulong());
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid binary value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

#endif // PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
