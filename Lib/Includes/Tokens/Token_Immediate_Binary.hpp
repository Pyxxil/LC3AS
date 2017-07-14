#ifndef TOKEN_IMMEDIATE_BINARY_HPP
#define TOKEN_IMMEDIATE_BINARY_HPP

#include "Tokens/Token_Immediate.hpp"

#include <bitset>

class Binary : public Immediate
{
public:
        Binary(std::string &immediate, std::string &immediate_uppercase, std::string &t_file, int line_number);
};

#endif // TOKEN_IMMEDIATE_BINARY_HPP
