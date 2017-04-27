#include "Tokens/Token_Label.hpp"

Label::Label(std::string &name, int line_number)
        : Token(name, name, line_number), label(name)
{}

std::int32_t Label::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        if (tokens.size() == 1) {
                return 0;
        }

        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());

        std::int32_t ret = vec.front()->assemble(vec, assembler);

        assembled = vec.front()->assembled;
        return ret;
}

bool Label::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return vec.front()->valid_arguments(vec);
        } else {
                return true;
        }
}

std::int32_t Label::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (tokens.size() > 1) {
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return vec.front()->guess_memory_size(vec);
        } else {
                return 0;
        }
}

void Label::not_found()
{
        std::cerr << "ERROR: ";
        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }
        std::cerr << "No such label '" << label << "'\n";
}

std::string Label::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                               std::uint16_t &program_counter,
                               const std::string &symbol,
                               const Assembler &assembler) const
{
        if (tokens.size() > 1) {
                std::vector<std::shared_ptr<Token>> vec(tokens);
                vec.erase(vec.begin());
                return tokens.at(1)->disassemble(vec, program_counter, symbol, assembler);
        } else {
                return "";
        }
}

Token::token_type Label::type() const
{
        return Token::token_type::LABEL;
}
