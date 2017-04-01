#ifndef PROJECT_TOKEN_STRING_HPP
#define PROJECT_TOKEN_STRING_HPP

#include "Token.hpp"

class String : public Token
{
public:
        String(std::string &string, int line_number = 0) : Token(string, line_number)
        {
                std::cout << '"' << string << '"';
        }

        Token::token_type type() override
        {
                return Token::token_type::_STRING;
        }

        Token expected(std::string expects) override
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "Expected " << expects << "." << std::endl;

                is_error = true;

                return *this;
        }
};

#endif //PROJECT_TOKEN_STRING_HPP
