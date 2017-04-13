#include <Tokens/All_Tokens.hpp>

String::String(std::string &string, int line_number) : Token(string, line_number)
{}

Token::token_type String::type() const
{
        return Token::token_type::_STRING;
}

Token &String::expected(const char *const expects)
{
        std::cerr << "ERROR: ";
        if (at_line) {
                std::cerr << "Line " << at_line << ": ";
        }
        std::cerr << "Expected " << expects << "." << std::endl;

        is_error = true;

        return *this;
}
