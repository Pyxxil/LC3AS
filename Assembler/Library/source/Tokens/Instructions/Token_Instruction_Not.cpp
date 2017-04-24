#include "Tokens/Instructions/Token_Instruction_Not.hpp"

#include <Assembler.hpp>
#include <Tokens/Token_Register.hpp>

Not::Not()
        : Instruction()
{}

Not::Not(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Not::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0x903F |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
                ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6))
        );

        return 1;
}

bool Not::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::REGISTER) {
                tokens.at(2)->expected("register");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Not::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Not::type() const
{
        return Token::token_type::OP_NOT;
}
