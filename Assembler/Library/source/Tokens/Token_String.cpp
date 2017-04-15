#include <Tokens/All_Tokens.hpp>

String::String(std::string &string, int line_number)
        : Token(string, line_number)
{
        for (std::size_t index = 0; index < string.size(); ++index) {
                if (string.at(index) == '\\') {
                        switch (string.at(index + 1)) {
                        case 'n':
                                ++index;
                                assembled.push_back('\n');
                                break;
                        case 't':
                                ++index;
                                assembled.push_back('\t');
                                break;
                        case 'v':
                                ++index;
                                assembled.push_back('\v');
                                break;
                        case '\\':
                        default:
                                assembled.push_back('\\');
                                break;
                        }
                } else {
                        assembled.push_back(static_cast<std::uint16_t>(string.at(index)));
                }
        }

        assembled.push_back('\0');
}

Token::token_type String::type() const
{
        return Token::_STRING;
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
