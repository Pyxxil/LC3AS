#ifndef TOKEN_ADDON_INCLUDE_HPP
#define TOKEN_ADDON_INCLUDE_HPP

#include "Token.hpp"

#include <string>

class Include : public Token
{
public:
  Include(std::string& t_token,
          std::string& t_token_uppercase,
          std::string& t_file,
          size_t line,
          size_t column);

  virtual bool valid_arguments(
    std::vector<std::shared_ptr<Token>>& tokens) override;

  virtual token_type type() const override;
};

#endif // TOKEN_ADDON_INCLUDE_HPP
