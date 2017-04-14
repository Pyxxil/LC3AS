#include <Tokens/All_Tokens.hpp>

String::String(std::string &string, int line_number)
        : Token(string, line_number)
{}

Token::token_type String::type() const
{
        return Token::token_type::_STRING;
}

Token &String::expected(const char *const expects)
{
        fprintf(stderr, "ERROR: ");
        if (at_line) {
                fprintf(stderr, "Line %d:", at_line);
        }
        fprintf(stderr, "Expected %s. Found '%s' instead.\n", expects, word.c_str());

        is_error = true;

        return *this;
}
