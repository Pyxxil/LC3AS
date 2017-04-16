#include "Tokens/Directives/Token_Directive_Fill.hpp"

#include <algorithm>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Fill::Fill(std::string &word, int line_number)
        : Directive(word, line_number), first_time(true)
{}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return 1;
        }

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
                std::cerr << ".FILL after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->is_error) {
                return -1;
        }

        if (tokens[1]->type() == Token::IMMEDIATE) {
                assembled.push_back(
                        static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate)
                );
        } else if (tokens[1]->type() == Token::LABEL) {
                if (!first_time) {
                        const auto symbol = std::find_if(
                                assembler.symbols.begin(), assembler.symbols.end(),
                                [&tokens](auto sym) -> bool
                                {
                                        return sym.second->word == tokens[1]->word;
                                }
                        );

                        if (symbol == assembler.symbols.end()) {
                                std::static_pointer_cast<Label>(tokens[1])->not_found();
                                return -1;
                        } else {
                                assembled.push_back(symbol->second->address);
                        }
                }
        } else {
                tokens[1]->expected("valid immediate value or label");
                return -1;
        }

        first_time = false;
        return 1;
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
