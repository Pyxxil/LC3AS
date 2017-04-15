#include "Tokens/Instructions/Token_Instruction_St.hpp"

#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"
#include <Assembler.hpp>

St::St(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t St::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 3) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("ST after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::LABEL) {
                tokens[2]->expected("label");
                return -1;
        }

        if (tokens[1]->is_error || tokens[2]->is_error) {
                return 0;
        }

        auto symbol = std::find_if(assembler.symbols.begin(), assembler.symbols.end(),
                                   [&tokens](auto sym) -> bool
                                   {
                                           return sym.second->word == tokens[2]->word;
                                   }
        );

        if (symbol == assembler.symbols.end()) {
                tokens[2]->expected("valid label");
                return -1;
        }

        int offset = static_cast<int>(symbol->second->address);
        offset -= (static_cast<int>(assembler.internal_program_counter) + 1);

        if (offset > 255 || offset < -256) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens[2]->expected("9 bit immediate value");
                return -1;
        }

        assembled.push_back(static_cast<std::uint16_t >(0x3000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) | (offset & 0x1FF))
        );

        return 1;
}

Token::token_type St::type() const
{
        return Token::token_type::OP_ST;
}
