#include "Tokens/Instructions/Token_Instruction_Ldi.hpp"

#include <algorithm>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Ldi::Ldi(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Ldi::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        int offset = 0;

        if (tokens.at(2)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(
                        assembler.symbols.cbegin(),
                        assembler.symbols.cend(),
                        [&tokens](auto &&sym) -> bool
                        {
                                return sym.second->word == tokens.at(2)->word;
                        }
                );

                if (symbol == assembler.symbols.end()) {
                        std::static_pointer_cast<Label>(tokens.at(2))->not_found();
                        return -1;
                }

                offset = static_cast<int>(symbol->second->address) -
                         (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(2))->immediate;
        }

        if (offset > 255 || offset < -256) {
                tokens.at(2)->expected("9 bit offset");
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t >(0xA000 |
                ((std::static_pointer_cast<Register>(tokens.at(1))->reg & 7) << 9) |
                (offset & 0x1FF))
        );

        return 1;
}

bool Ldi::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return (is_valid = false);
        } else if (tokens[2]->type() != Token::LABEL && tokens.at(2)->type() != Token::IMMEDIATE) {
                tokens[2]->expected("label or immediate value");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Ldi::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Ldi::type() const
{
        return Token::token_type::OP_LDI;
}
