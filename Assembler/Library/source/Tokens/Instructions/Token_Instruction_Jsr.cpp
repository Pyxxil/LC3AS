#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"

#include <algorithm>

#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Jsr::Jsr(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Jsr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
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
                std::cerr << "ADD after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::LABEL) {
                tokens[1]->expected("label");
                return -1;
        }

        auto symbol = std::find_if(assembler.symbols.begin(), assembler.symbols.end(),
                                   [&tokens](auto sym) -> bool
                                   {
                                           return sym.second->word == tokens[1]->word;
                                   }
        );

        if (symbol == assembler.symbols.end()) {
                tokens[1]->expected("valid label");
                return -1;
        }

        int offset = static_cast<int>(symbol->second->address);
        offset -= (static_cast<int>(assembler.internal_program_counter) + 1);

        if (offset > 1023 || offset < -1024) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens[1]->expected("11 bit immediate value");
                return -1;
        }

        assembled.push_back(
                static_cast<std::uint16_t>(0x4800 | (offset & 0x7FF))
        );

        return 1;
}

Token::token_type Jsr::type() const
{
        return Token::token_type::OP_JSR;
}

std::vector<std::uint16_t> Jsr::as_assembled()
{
        return Token::as_assembled();
}
