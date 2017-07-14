#include "Tokens/Tokens.hpp"

#include <sstream>

#include "Diagnostics.hpp"

String::String(std::string &string, std::string &t_file, int line_number)
        : Token(string, string, t_file, line_number)
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
                        case '"':
                                ++index;
                                assembled.emplace_back('"');
                                break;
                        case '\\':
                                assembled.emplace_back('\\');
                                break;
                        default:
                                std::stringstream ss;
                                ss << "Invalid character escape sequence ('\\" << string.at(index + 1)
                                   << "'). Ignoring the '\\'.";
                                // TODO: Fix this, requires all things to know the 'column' in which they were found.
                                /*Logging::logger->LOG(
                                        Logging::WARNING, line_number, t_file, ss.str(), Logging::WARNING_TYPE::SYNTAX
                                );*/
                                assembled.emplace_back(string.at(index + 1));
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
