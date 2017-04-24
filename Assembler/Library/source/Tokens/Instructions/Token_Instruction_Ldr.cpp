#include "Tokens/Instructions/Token_Instruction_Ldr.hpp"

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Ldr::Ldr(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Ldr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0x6000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
                ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6) |
                (std::static_pointer_cast<Immediate>(tokens[3])->immediate & 0x3F))
        );

        return 1;
}

bool Ldr::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 4) {
                invalid_argument_count(tokens.size(), 3);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::REGISTER) {
                tokens.at(2)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(3)->type() != Token::IMMEDIATE) {
                tokens.at(3)->expected("immediate value");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid && tokens.at(3)->is_valid)) {
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(3))->immediate > 31 ||
            std::static_pointer_cast<Immediate>(tokens.at(3))->immediate < -32) {
                tokens.at(3)->expected("6 bit offset");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Ldr::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Ldr::type() const
{
        return Token::token_type::OP_LDR;
}
