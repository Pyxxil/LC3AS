#include "Tokens/Additional/Token_Addon_Sub.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Sub::Sub()
        : Directive()
{
        set_zero     = std::make_shared<And>();
        decimal_zero = std::make_shared<Decimal>("#0");
        neg1         = std::make_shared<Neg>();
        neg2         = std::make_shared<Neg>();
        add          = std::make_shared<Add>();
}

Sub::Sub(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{
        std::string neg = "NEG";

        set_zero     = std::make_shared<And>();
        decimal_zero = std::make_shared<Decimal>("#0");
        neg1         = std::make_shared<Neg>(neg, neg, line_number);
        neg2         = std::make_shared<Neg>(neg, neg, line_number);
        add          = std::make_shared<Add>();

        add->at_line = set_zero->at_line = decimal_zero->at_line = line_number;
}

std::int32_t Sub::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return 0;
        }

        std::int32_t ret = 0;

        std::size_t first_register_index  = 1;
        std::size_t second_register_index = 2;

        if (tokens.size() == 4) {
                ++first_register_index;
                ++second_register_index;
        }

        if (std::static_pointer_cast<Register>(tokens.at(first_register_index))->reg ==
            std::static_pointer_cast<Register>(tokens.at(second_register_index))->reg) {
                std::vector<std::shared_ptr<Token>> vec = {
                        set_zero, tokens.at(1), tokens.at(first_register_index), decimal_zero
                };

                set_zero->assemble(vec, assembler);
                assembled = set_zero->assembled;

                ret = 1;
        } else {
                std::vector<std::shared_ptr<Token>> vec = {neg1, tokens.at(second_register_index)};
                ret += neg1->assemble(vec, assembler);

                vec = {add, tokens.at(1), tokens.at(first_register_index), tokens.at(second_register_index)};
                ret += add->assemble(vec, assembler);

                assembled = neg1->assembled;
                for (const auto &as_assembled : add->assembled) {
                        assembled.emplace_back(as_assembled);
                }

                vec = {neg2, tokens.at(second_register_index)};
                ret += neg2->assemble(vec, assembler);

                for (const auto &as_assembled : neg2->assembled) {
                        assembled.emplace_back(as_assembled);
                }
        }

        return ret;
}

bool Sub::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 4 || tokens.size() == 1) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::REGISTER) {
                tokens.at(2)->expected("register");
                return (is_valid = false);
        }

        if (tokens.size() == 4) {
                if (tokens.at(3)->type() != Token::REGISTER) {
                        tokens.at(3)->expected("register");
                        return (is_valid = false);
                } else if (!tokens.at(3)->is_valid) {
                        return (is_valid = false);
                }
        }

        return (is_valid = tokens.at(1)->is_valid && tokens.at(2)->is_valid);
}

std::int32_t Sub::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (is_valid) {
                if (std::static_pointer_cast<Register>(tokens.at(1))->reg ==
                    std::static_pointer_cast<Register>(tokens.at(2))->reg) {
                        return 1;
                } else {
                        return 5;
                }
        } else {
                return -1;
        }
}

void Sub::invalid_argument_count(std::size_t provided, std::size_t expected) const
{
        (void) expected;

        provided -= 1;

        std::cerr << "ERROR: ";

        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }

        std::cerr << ".SUB expects 2 or 3 arguments, but " << provided
                  << (provided == 1 ? " argument was " : "arguments were ") << "provided.\n";
}

std::string Sub::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                             std::uint16_t &program_counter,
                             const std::string &symbol,
                             const Assembler &assembler) const
{
        std::stringstream stream;

        std::size_t first_register_index  = 1;
        std::size_t second_register_index = 2;

        if (tokens.size() == 4) {
                ++first_register_index;
                ++second_register_index;
        }

        if (std::static_pointer_cast<Register>(tokens.at(first_register_index))->reg ==
            std::static_pointer_cast<Register>(tokens.at(second_register_index))->reg) {
                // If the registers are the same, then it's more efficient to just erase the value in
                // that register.
                std::vector<std::shared_ptr<Token>> vec = {
                        set_zero, tokens.at(1), tokens.at(first_register_index), decimal_zero
                };
                stream << set_zero->disassemble(vec, program_counter, symbol, assembler);
        } else {
                std::vector<std::shared_ptr<Token>> vec = {neg1, tokens.at(second_register_index)};
                stream << neg1->disassemble(vec, program_counter, symbol, assembler);

                vec = {add, tokens.at(1), tokens.at(first_register_index), tokens.at(second_register_index)};
                stream << add->disassemble(vec, program_counter, " ", assembler);

                if (second_register_index == 2 ||
                    std::static_pointer_cast<Register>(tokens.at(1))->reg !=
                    std::static_pointer_cast<Register>(tokens.at(second_register_index))->reg) {
                        // If the last provided register is the same as the destination register, then we don't want
                        // to turn the last register back into it's original value.
                        vec = {neg2, tokens.at(second_register_index)};
                        stream << neg2->disassemble(vec, program_counter, " ", assembler);
                }
        }

        return stream.str();
}

Token::token_type Sub::type() const
{
        return Token::ADDON_SUB;
}
