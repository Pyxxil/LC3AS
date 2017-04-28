#ifndef PROJECT_TOKEN_ADDON_CHAR_HPP
#define PROJECT_TOKEN_ADDON_CHAR_HPP

#include "Tokens/Token_Immediate.hpp"

class Character : public Immediate
{
public:
        Character();
        Character(std::string &character, int line_number = 0);
};

#endif //PROJECT_TOKEN_ADDON_CHAR_HPP
