#include "Tokens/Additional/Token_Addon_Sub.hpp"

#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_And.hpp"
#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"
#include "Tokens/Additional/Token_Addon_Neg.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Sub::Sub()
        : Token()
{

}

Sub::Sub(std::string &word, int line_number)
        : Token(word, line_number)
{

}

// TODO: Change this so you can have it similar to ADD R1, R2, R3
std::int32_t Sub::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return 0;
        }

        std::int32_t ret = 0;

        if (std::static_pointer_cast<Register>(tokens[1])->reg == std::static_pointer_cast<Register>(tokens[2])->reg) {
                std::shared_ptr<Token> set_zero     = std::make_shared<And>();
                std::shared_ptr<Token> decimal_zero = std::make_shared<Decimal>("#0");

                std::vector<std::shared_ptr<Token>> vec = {set_zero, tokens[1], tokens[2], decimal_zero};

                set_zero->assemble(vec, assembler);
                assembled.swap(set_zero->assembled);

                ret = 1;
        } else {
                std::shared_ptr<Token> neg1 = std::make_shared<Neg>();
                std::shared_ptr<Token> neg2 = std::make_shared<Neg>();

                std::vector<std::shared_ptr<Token>> vec = {neg1, tokens[2]};
                ret += neg1->assemble(vec, assembler);

                std::shared_ptr<Token> add = std::make_shared<Add>();
                vec = {add, tokens[1], tokens[1], tokens[2]};
                ret += add->assemble(vec, assembler);

                assembled.swap(neg1->assembled);
                for (const auto &as_assembled : add->assembled) {
                        assembled.emplace_back(as_assembled);
                }

                vec = {neg2, tokens[2]};
                ret += neg2->assemble(vec, assembler);

                for (const auto &as_assembled : neg2->assembled) {
                        assembled.emplace_back(as_assembled);
                }
        }

        return ret;
}

bool Sub::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
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

Token::token_type Sub::type() const
{
        return Token::ADDON_SUB;
}
