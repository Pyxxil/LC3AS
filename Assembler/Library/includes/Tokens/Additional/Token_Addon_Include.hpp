#ifndef TOKEN_ADDON_INCLUDE_HPP
#define TOKEN_ADDON_INCLUDE_HPP

#include "Token.hpp"

#include <string>

class Include : public Token
{
public:
        Include();
        Include(std::string &t_token, std::string &t_token_uppercase, std::string &t_file, int line);
};

#endif // TOKEN_ADDON_INCLUDE_HPP
