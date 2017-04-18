#include "Tokens/Additional/Token_Addon_Sub.hpp"

#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Additional/Token_Addon_Neg.hpp"
#include "Assembler.hpp"

Sub::Sub()
        : Token()
{

}

Sub::Sub(std::string &word, int line_number)
        : Token(word, line_number)
{

}

// TODO: Change this so you can have it like ADD R1, R2, R3 or ADD R1, R2, #2
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

        std::shared_ptr<Token> neg1 = std::make_shared<Neg>();
        std::vector<std::shared_ptr<Token>> vec = {neg1, tokens[2]};
        ret += neg1->assemble(vec, assembler);

        std::shared_ptr<Token> add = std::make_shared<Add>();
        vec = {add, tokens[1], tokens[1], tokens[2]};
        ret += add->assemble(vec, assembler);

        assembled.swap(neg1->assembled);
        for (const auto &as_assembled : add->assembled) {
                assembled.push_back(as_assembled);
        }

        std::shared_ptr<Token> neg2 = std::make_shared<Neg>();
        vec = {neg2, tokens[2]};
        ret += neg2->assemble(vec, assembler);

        for (const auto &as_assembled : neg2->assembled) {
                assembled.push_back(as_assembled);
        }

        return ret;
}

Token::token_type Sub::type() const
{
        return Token::ADDON_SUB;
}
