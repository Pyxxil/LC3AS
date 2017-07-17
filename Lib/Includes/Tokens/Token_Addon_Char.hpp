#ifndef TOKEN_ADDON_CHAR_HPP
#define TOKEN_ADDON_CHAR_HPP

#include "Tokens/Token_Immediate.hpp"

class Character: public Immediate
{
public:
    Character() = default;
    Character(std::string &character,
              std::string &t_file,
              size_t line_number,
              size_t column);
};

#endif // TOKEN_ADDON_CHAR_HPP
