#include "Tokens/Directives/Token_Directive_Blkw.hpp"

#include <algorithm>

#include "Assembler.hpp"

Blkw::Blkw(std::string &word, int line_number)
        : Directive(word, line_number)
{}

std::int32_t Blkw::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << ".BLKW after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens.size() > 3 || !tokens.size()) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (tokens[1]->type() != Token::IMMEDIATE) {
                tokens[1]->expected("immediate value");
                return -1;
        } else if (tokens[1]->is_error) {
                return -1;
        }

        std::uint16_t fill = 0;

        bool do_fill = true;

        if (tokens.size() == 3) {
                if (tokens[2]->is_error) {
                        return -1;
                }

                if (tokens[2]->type() == Token::IMMEDIATE) {
                        fill = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[2])->immediate);
                } else if (tokens[2]->type() == Token::LABEL) {
                        if (!first_time) {
                                const auto label = std::find_if(
                                        assembler.symbols.begin(), assembler.symbols.end(),
                                        [&tokens](auto symbol) -> bool
                                        {
                                                return symbol.second->word == tokens[2]->word;
                                        }
                                );

                                if (label == assembler.symbols.end()) {
                                        std::static_pointer_cast<Label>(tokens[2])->not_found();
                                        return -1;
                                }

                                fill = label->second->address;
                        } else {
                                do_fill = false;
                        }
                } else {
                        tokens[2]->expected("either an immediate value or label");
                        return -1;
                }
        }

        if (do_fill) {
                for (std::uint16_t block = 0; block < std::static_pointer_cast<Immediate>(tokens[1])->immediate;
                     ++block) {
                        assembled.push_back(fill);
                }
        }

        first_time = false;
        return std::static_pointer_cast<Immediate>(tokens[1])->immediate;
}

void Blkw::invalid_argument_count(std::size_t provided, std::size_t expected)
{
        (void) expected;

        provided -= 1;

        std::cerr << "ERROR: ";
        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }
        std::cerr << ".BLKW expects 1 or 2 arguments, but " << provided
                  << " argument" << (provided == 1 ? "" : "'s") << " provided.\n",

                is_error = true;
}

Token::token_type Blkw::type() const
{
        return Token::token_type::DIR_BLKW;
}
