#ifndef PROJECT_TOKEN_OPERATOR_ADD_HPP
#define PROJECT_TOKEN_OPERATOR_ADD_HPP

#include "../Token_Instruction.hpp"

class Add : public Instruction
{
public:
        Add(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_ADD;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 4) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
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

                assembled.push_back(static_cast<std::uint16_t> (
                        0x1000 | static_cast<std::uint16_t >(((static_cast<Register *>(tokens[1])->reg & 7) << 9) |
                                ((static_cast<Register *>(tokens[2])->reg) & 0x7) << 6)
                ));

                if (tokens[3]->type() == Token::REGISTER) {
                        assembled.front() |= (static_cast<Register *>(tokens[3])->reg & 0x7);
                } else {
                        assembled.front() |= (0x20 |
                                static_cast<std::uint16_t>(static_cast<Immediate *>(tokens[3])->immediate & 0x1F));
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_ADD_HPP
