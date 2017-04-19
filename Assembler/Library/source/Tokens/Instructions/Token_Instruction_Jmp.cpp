#include "Tokens/Instructions/Token_Instruction_Jmp.hpp"
#include "Tokens/Token_Register.hpp"

#include "Assembler.hpp"

Jmp::Jmp(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Jmp::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
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
                std::cerr << "JMP after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[1]->is_error) {
                return -1;
        }

        assembled.emplace_back(
                static_cast<std::uint16_t>(
                        0xC000 |
                        (std::static_pointer_cast<Register>(tokens[1])->reg << 5)
                )
        );

        return 1;
}

Token::token_type Jmp::type() const
{
        return Token::token_type::OP_JMP;
}
