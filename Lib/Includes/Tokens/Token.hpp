#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

#include "Symbol.hpp"

#define SIGNED true
#define UNSIGNED false

class Token
{
public:
        Token() = default;
        Token(std::string &t_token, std::string &t_token_uppercase, std::string &t_file, size_t line, size_t column);
        Token(const Token &other) = default;
        Token(Token &&other) noexcept = default;

        Token &operator =(const Token &rhs) = default;
        Token &operator =(Token &&rhs) noexcept = default;

        virtual ~Token() = default;

        std::string token;
        std::string token_uppercase;
        std::string file;

        std::vector<uint16_t> assembled { };

        size_t at_line   = 0;
        size_t at_column = 0;
        bool   is_valid  = true;

        enum token_type
        {
                NONE = 0,
                REGISTER,
                LABEL,
                IMMEDIATE,
                DIR_STRINGZ,
                DIR_FILL,
                DIR_BLKW,
                DIR_ORIG,
                DIR_END,
                OP_AND,
                OP_ADD,
                OP_BR,
                OP_JMP,
                OP_RET,
                OP_JSR,
                OP_JSRR,
                OP_RTI,
                OP_NOT,
                OP_LDI,
                OP_LEA,
                OP_LDR,
                OP_LD,
                OP_ST,
                OP_STI,
                OP_STR,
                OP_TRAP,
                TRAP_PUTS,
                TRAP_GETC,
                TRAP_IN,
                TRAP_OUT,
                TRAP_PUTSP,
                TRAP_HALT,
                _STRING,
#ifdef INCLUDE_ADDONS
                ADDON_NEG,
                ADDON_SUB,
                ADDON_SET,
                ADDON_LSHIFT,
                ADDON_INCLUDE,
#endif
        };

        virtual Token::token_type type() const;

        std::string deduce_type() const;
        virtual std::string disassemble(uint16_t &program_counter,
                                        const std::string &symbol,
                                        int width) const;

        virtual void expected(const char *expects) const;
        virtual void invalid_argument_count(size_t provided, size_t expected) const;
        virtual void requires_too_many_bits(int allowed_bits,
                                            bool is_signed,
                                            const Token *const caller,
                                            const std::map<std::string, Symbol> &symbols)
        {
                (void) allowed_bits;
                (void) is_signed;
                (void) caller;
                (void) symbols;
        }

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                                      const std::map<std::string, Symbol> &symbols,
                                      uint16_t program_counter);
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const;

        virtual const std::vector<uint16_t> as_assembled() const;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens);
};

#endif // TOKEN_HPP
