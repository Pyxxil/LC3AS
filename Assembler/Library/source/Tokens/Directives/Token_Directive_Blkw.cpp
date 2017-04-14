#include "Tokens/Directives/Token_Directive_Blkw.hpp"

#include "Assembler.hpp"

Blkw::Blkw(std::string &word, int line_number)
        : Directive(word, line_number)
{}

std::int32_t Blkw::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (tokens.size() != 2) {
                return -1;
        }

        if (tokens[1]->type() != Token::IMMEDIATE || tokens[1]->is_error) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                return 0;
        }

        for (std::uint16_t block = 0; block < std::static_pointer_cast<Immediate>(tokens[1])->immediate; ++block) {
                assembled.push_back(0);
        }

        return std::static_pointer_cast<Immediate>(tokens[1])->immediate;
}

Token::token_type Blkw::type() const
{
        return Token::token_type::DIR_BLKW;
}
