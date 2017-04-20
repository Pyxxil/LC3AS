#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"

#include <algorithm>

#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Jsr::Jsr(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Jsr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        const auto &&symbol = std::find_if(
                assembler.symbols.cbegin(), assembler.symbols.cend(),
                [&tokens](const auto &sym) -> bool
                {
                        return sym.second->word == tokens[1]->word;
                }
        );

        if (symbol == assembler.symbols.end()) {
                tokens[1]->expected("valid label");
                return -1;
        }

        const int offset = static_cast<int>(symbol->second->address) -
                (static_cast<int>(assembler.internal_program_counter) + 1);

        if (offset > 1023 || offset < -1024) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens.at(1)->expected("11 bit immediate value");
                return -1;
        }

        assembled.emplace_back(
                static_cast<std::uint16_t>(0x4800 | (offset & 0x7FF))
        );

        return 1;
}

bool Jsr::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::LABEL) {
                tokens.at(1)->expected("label");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Jsr::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Jsr::type() const
{
        return Token::token_type::OP_JSR;
}

const std::vector<std::uint16_t> Jsr::as_assembled() const
{
        return Token::as_assembled();
}
