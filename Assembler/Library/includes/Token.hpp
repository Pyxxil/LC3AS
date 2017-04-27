#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>


// TODO: Fix the fact that parameter names aren't consistent across all token files.
// TODO: Also fix the positioning of method declarations.

class Assembler;

class Token
{
public:
        Token();
        Token(std::string &token, std::string &uppercased, int line);

        virtual ~Token() = default;

        std::string word;
        std::string word_as_uppercase;

        std::vector<std::uint16_t> assembled;

        int  at_line  = 0;
        bool is_valid = true;

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
#endif
        };

        virtual Token::token_type type() const;

        std::string deduce_type() const;
        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const;

        virtual void expected(const char *const expects) const;
        virtual void invalid_argument_count(std::size_t provided, std::size_t expected) const;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler);
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const;

        virtual const std::vector<uint16_t> as_assembled() const;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens);

        void unterminated(std::string &&t_type);
};

#endif // LC3_SIMULATOR_TOKEN_HPP
