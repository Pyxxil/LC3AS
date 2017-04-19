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
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
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
                std::cerr << "NOT after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::REGISTER) {
                tokens[2]->expected("register");
                return -1;
        }

        if (tokens[1]->is_error || tokens[2]->is_error) {
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
                for (auto &&as_assembled : add->assembled) {
                        assembled.emplace_back(as_assembled);
                }

                vec = {neg2, tokens[2]};
                ret += neg2->assemble(vec, assembler);

                for (auto &&as_assembled : neg2->assembled) {
                        assembled.emplace_back(as_assembled);
                }
        }

        return ret;
}

Token::token_type Sub::type() const
{
        return Token::ADDON_SUB;
}
