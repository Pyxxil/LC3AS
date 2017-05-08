#include <Token.hpp>
#include "Tokens/Additional/Token_Addon_Include.hpp"

Include::Include()
{

}

Include::Include(std::string &t_token, std::string &t_token_uppercase, std::string &t_file, int line)
        : Token(t_token, t_token_uppercase, t_file, line)
{

}
