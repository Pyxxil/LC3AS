#ifndef PROJECT_TOKEN_ADDON_SUB_HPP
#define PROJECT_TOKEN_ADDON_SUB_HPP

#include <Token.hpp>

class Sub : public Token
{
public:
        Sub();
        Sub(std::string &word, int line_number = 0);
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual token_type type() const override;
};

#endif //PROJECT_TOKEN_ADDON_SUB_HPP
