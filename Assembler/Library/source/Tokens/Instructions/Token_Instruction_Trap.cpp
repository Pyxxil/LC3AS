#include "Tokens/Instructions/Token_Instruction_Trap.hpp"

#include "Tokens/Token_Immediate.hpp"
#include "Assembler.hpp"

Trap::Trap(std::string &token, int line_number)
        : Instruction(token, line_number)
{}

std::int32_t Trap::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
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
                std::cerr << "TRAP after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::IMMEDIATE) {
                tokens[1]->expected("immediate value");
                return -1;
        } else if (tokens[1]->is_error) {
                return -1;
        }

        if (std::static_pointer_cast<Immediate>(tokens[1])->immediate > 0xFF) {
                tokens[1]->expected("8 bit trap vector");
                return -1;
        }

        if (std::static_pointer_cast<Immediate>(tokens[1])->immediate > 0x25 ||
            std::static_pointer_cast<Immediate>(tokens[1])->immediate < 0x20) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "TRAP supplied " << std::static_pointer_cast<Immediate>(tokens[1])->immediate
                          << ", which is possibly an illegal trap vector";
        }

        assembled.emplace_back(
                static_cast<std::uint16_t>(
                        0xF000 |
                        (std::static_pointer_cast<Immediate>(tokens[1])->immediate & 0xFF)
                )
        );

        return 1;
}

Token::token_type Trap::type() const
{
        return Token::token_type::OP_TRAP;
}
