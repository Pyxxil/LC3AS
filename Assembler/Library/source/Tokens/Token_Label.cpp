#include "Tokens/Token_Label.hpp"

Label::Label(std::string &name, int line_number)
        : Token(name, line_number), label(name)
{}

std::int32_t Label::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() == 1) {
                return 0;
        }

        if (tokens[1]->assembled.size()) {
                return static_cast<std::int32_t>(tokens[1]->assembled.size());
        }

        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
        std::int32_t ret = vec.front()->assemble(vec, assembler);
        assembled = vec.front()->assembled;
        return ret;
}

void Label::not_found()
{
        fprintf(stderr, "ERROR: ");
        if (at_line) {
                fprintf(stderr, "Line %d:", at_line);
        }
        fprintf(stderr, "No such label '%s'\n", label.c_str());

        is_error = true;
}

Token::token_type Label::type() const
{
        return Token::token_type::LABEL;
}
