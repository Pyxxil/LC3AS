#ifndef PROJECT_TOKEN_LABEL_HPP
#define PROJECT_TOKEN_LABEL_HPP

#include "Token.hpp"

class Label : public Token
{

        // TODO: Add some sort of fuzzy match to check if the user meant something else
public:
        Label(std::string &name, int line_number) : Token(name, line_number), label(name)
        {}

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() == 1) {
                        return 0;
                }

                std::vector<Token *> vec(tokens.begin() + 1, tokens.end());
                std::int32_t ret = vec.front()->assemble(vec, orig_seen, end_seen);
                assembled = vec.front()->assembled;
                return ret;
        }

        std::string label;
        std::uint16_t address = 0;

        virtual Token::token_type type() const override
        { return Token::token_type::LABEL; }

        void not_found()
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "No such label '" << label << "'\n";

                is_error = true;
        }
};

#endif //PROJECT_TOKEN_LABEL_HPP
