#include <Tokens/All_Tokens.hpp>

String::String(std::string &string, int line_number)
        : Token(string, line_number)
{
        for (std::size_t index = 0; index < string.size(); ++index) {
                if (string.at(index) == '\\') {
                        switch (string.at(index + 1)) {
                        case 'n':
                                ++index;
                                assembled.emplace_back('\n');
                                break;
                        case 't':
                                ++index;
                                assembled.emplace_back('\t');
                                break;
                        case 'v':
                                ++index;
                                assembled.emplace_back('\v');
                                break;
                        case '\\':
                        default:
                                assembled.emplace_back('\\');
                                break;
                        }
                } else {
                        assembled.emplace_back(static_cast<std::uint16_t>(string.at(index)));
                }
        }

        assembled.emplace_back('\0');
}

Token::token_type String::type() const
{
        return Token::_STRING;
}
