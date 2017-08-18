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
          size_t line,
          size_t column);

  virtual bool valid_arguments(
    std::vector<std::shared_ptr<Token>>& tokens) override final;

  virtual token_type type() const override final;
};

#endif // TOKEN_ADDON_INCLUDE_HPP
