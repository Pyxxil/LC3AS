#ifndef PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
#define PROJECT_TOKEN_IMMEDIATE_BINARY_HPP

#include "Tokens/Token_Immediate.hpp"

#include <bitset>

class Binary : public Immediate
{
public:
        Binary(std::string &immediate, std::string &immediate_uppercase, int line_number = 0);
};

#endif // PROJECT_TOKEN_IMMEDIATE_BINARY_HPP
