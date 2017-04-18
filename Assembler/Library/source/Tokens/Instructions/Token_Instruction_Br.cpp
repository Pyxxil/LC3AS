#include "Tokens/Instructions/Token_Instruction_Br.hpp"

#include <algorithm>

#include "Assembler.hpp"

Br::Br(std::string &oper, int line_number, bool n, bool z, bool p)
        : Instruction(oper, line_number), N(n), Z(z), P(p)
{}

Token::token_type Br::type() const
{
        return Token::OP_BR;
}

std::int32_t Br::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "BR after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::LABEL) {
                tokens[1]->expected("label");
                return -1;
        }

        const auto &&symbol = std::find_if(assembler.symbols.cbegin(), assembler.symbols.cend(),
                                           [&tokens](const auto &sym) -> bool
                                           {
                                                   return sym.second->word == tokens[1]->word;
                                           }
        );

        if (symbol == assembler.symbols.end()) {
                std::static_pointer_cast<Label>(tokens[1])->not_found();
                return -1;
        }

        int offset = static_cast<int>(symbol->second->address);
        offset -= (static_cast<int>(assembler.internal_program_counter) + 1);

        if (offset > 255 || offset < -256) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens[1]->expected("9 bit immediate value");
                return -1;
        }

        assembled.push_back(
                static_cast<std::uint16_t>(0x0000 | N << 11 | Z << 10 | P << 9 | (offset & 0x1FF))
        );

        return 1;
}
