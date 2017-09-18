#ifndef TOKEN_ADDON_INCLUDE_HPP
#define TOKEN_ADDON_INCLUDE_HPP

#include "Tokens/Token.hpp"

#include <string>

/**
 * Include looks like so:
 * .INCLUDE <string>
 * The string is taken to be a file name, the contents of which (assumong it
 * exists) will also be lexed into the current stream.
 */

class Include : public Token
{
public:
  Include(const std::string& t_token,
          const std::string& t_token_uppercase,
          const std::string& t_file,
          size_t t_line,
          size_t t_column)
    : Token(t_token, t_token_uppercase, t_file, t_line, t_column)
  {}

  bool valid_arguments(std::vector<std::shared_ptr<Token>>& tokens) final;

  inline token_type type() const final { return ADDON_INCLUDE; }
};

#endif // TOKEN_ADDON_INCLUDE_HPP
