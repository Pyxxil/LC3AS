#include "Tokens/Instructions/Token_Instruction_Br.hpp"

#include <algorithm>

#include "Tokens/Token_Immediate.hpp"
#include "Assembler.hpp"

Br::Br(std::string &oper, int line_number, bool n, bool z, bool p)
        : Instruction(oper, line_number), N(n), Z(z), P(p)
{}

std::int32_t Br::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        int offset = 0;

        if (tokens.at(1)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(
                        assembler.symbols.cbegin(),
                        assembler.symbols.cend(),
                        [&tokens](auto &&sym) -> bool
                        {
                                return sym.second->word == tokens[1]->word;
                        }
                );

                if (symbol == assembler.symbols.end()) {
                        std::static_pointer_cast<Label>(tokens[1])->not_found();
                        return -1;
                }

                offset = static_cast<int>(symbol->second->address) -
                        (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(1))->immediate;
        }

        if (offset > 255 || offset < -256) {
                tokens.at(1)->expected("9 bit offset");
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0x0000 | N << 11 | Z << 10 | P << 9 | (offset & 0x1FF)));

        return 1;
}

bool Br::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{

        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::LABEL && tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("label or immediate value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Br::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<int32_t>(is_valid);
}

Token::token_type Br::type() const
{
        return Token::OP_BR;
}
