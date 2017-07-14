#ifndef TOKEN_ADDON_CHAR_HPP
#define TOKEN_ADDON_CHAR_HPP

#include "Tokens/Token_Immediate.hpp"

class Character : public Immediate
{
public:
        Character();
        Character(std::string &character, std::string &t_file, int line_number);
};

#endif // TOKEN_ADDON_CHAR_HPP
