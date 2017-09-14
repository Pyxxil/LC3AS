#include "Tokens/Addons/Token_Addon_Include.hpp"
#include "Tokens/Token.hpp"

Include::Include(const std::string& t_token,
                 const std::string& t_token_uppercase,
                 const std::string& t_file,
                 size_t line,
                 size_t column)
  : Token(t_token, t_token_uppercase, t_file, line, column)
{}

bool
Include::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::_STRING || !tokens[1]->is_valid) {
    tokens[1]->expected("string");
    return (is_valid = false);
  }

  return is_valid;
}
