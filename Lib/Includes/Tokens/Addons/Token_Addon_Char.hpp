#ifndef TOKEN_ADDON_CHAR_HPP
#define TOKEN_ADDON_CHAR_HPP

#include "Tokens/Token_Immediate.hpp"

/**
 * A simple character which is defined as
 * -?'\\?.'
 * Or, in english, a character is surrounded by single quotes, is 1 character
 * wide (with an optional '\' for extra characters like '\n'), as well as an
 * optional dash to symbolise that it's the negative version of it.
 *
 */

class Character : public Immediate
{
public:
  Character() = default;
  Character(std::string& character,
            std::string& t_file,
            size_t line_number,
            size_t column);
};

#endif // TOKEN_ADDON_CHAR_HPP
