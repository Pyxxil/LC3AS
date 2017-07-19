#include "Tokens/Token_Addon_Include.hpp"
#include "Tokens/Token.hpp"

Include::Include(std::string &t_token, std::string &t_token_uppercase,
                 std::string &t_file, size_t line, size_t column)
    : Token(t_token, t_token_uppercase, t_file, line, column)
{
}

bool Include::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
    if (tokens.size() != 2) {
        invalid_argument_count(tokens.size(), 1,
                               tokens.back()->column
                                   + tokens.back()->token.length());
        return (is_valid = false);
    }

    if (tokens.at(1)->type() != Token::_STRING || !tokens.at(1)->is_valid) {
        tokens.at(1)->expected("string");
        return (is_valid = false);
    }

    return is_valid;
}

Token::token_type Include::type() const { return Token::ADDON_INCLUDE; }
