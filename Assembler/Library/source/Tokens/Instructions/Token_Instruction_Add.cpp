#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Immediate.hpp"

#include <Assembler.hpp>

Add::Add(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Add::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 4) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before %s, %s will be ignored", word.c_str(), word.c_str());
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::REGISTER) {
                tokens[2]->expected("register");
                return -1;
        } else if (tokens[3]->type() != Token::REGISTER && tokens[3]->type() != Token::IMMEDIATE) {
                tokens[3]->expected("register or immediate value");
                return -1;
        }

        if (tokens[1]->is_error) {
                tokens[1]->expected("valid register");
                return -1;
        } else if (tokens[2]->is_error) {
                tokens[2]->expected("valid register");
                return -1;
        } else if (tokens[3]->is_error) {
                if (tokens[3]->type() == Token::REGISTER) {
                        expected("valid register");
                } else {
                        expected("valid immediate value");
                }
                return -1;
        }

        if (tokens[3]->type() == Token::IMMEDIATE) {
                if (std::static_pointer_cast<Immediate>(tokens[3])->immediate > 15 ||
                        std::static_pointer_cast<Immediate>(tokens[3])->immediate < -16) {
                        tokens[3]->expected("5 bit immediate value");
                        return -1;
                }
        }

        assembled.push_back(static_cast<std::uint16_t> (
                                    0x1000 | static_cast<std::uint16_t >(
                                            ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9) |
                                                    ((std::static_pointer_cast<Register>(tokens[2])->reg) & 0x7) << 6)
                            ));

        if (tokens[3]->type() == Token::REGISTER) {
                assembled.front() |= (std::static_pointer_cast<Register>(tokens[3])->reg & 0x7);
        } else {
                assembled.front() |= (0x20 |
                        static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[3])->immediate & 0x1F));
        }

        return 1;
}

Token::token_type Add::type() const
{
        return Token::token_type::OP_ADD;
}
