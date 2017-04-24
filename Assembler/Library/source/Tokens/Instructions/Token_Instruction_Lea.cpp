#include "Tokens/Instructions/Token_Instruction_Lea.hpp"

#include <algorithm>

#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Lea::Lea(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Lea::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        const auto symbol = std::find_if(
                assembler.symbols.begin(), assembler.symbols.end(),
                [&tokens](auto sym) -> bool
                {
                        return sym.second->word == tokens[2]->word;
                }
        );

        if (symbol == assembler.symbols.end()) {
                std::static_pointer_cast<Label>(tokens[2])->not_found();
                return -1;
        }

        const int offset = static_cast<int>(symbol->second->address) -
                           (static_cast<int>(assembler.internal_program_counter) + 1);

        if (offset > 255 || offset < -256) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens[2]->expected("9 bit immediate value");
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t >(0xE000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9) |
                (offset & 0x1FF))
        );

        return 1;
}

bool Lea::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::LABEL) {
                tokens.at(2)->expected("label");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Lea::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Lea::type() const
{
        return Token::token_type::OP_LEA;
}

const std::vector<std::uint16_t> Lea::as_assembled() const
{
        return Token::as_assembled();
}
