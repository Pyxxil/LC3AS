#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <iostream>

inline void ERROR(const char *const str, ...)
{
        va_list args;
        va_start(args, str);
        fprintf(stderr, "ERROR: ");
        fprintf(stderr, str, args);
        fprintf(stderr, ".\n");
        va_end(args);
}

inline void WARNING(const char *const str, ...)
{
        va_list args;
        va_start(args, str);
        fprintf(stderr, "WARNING: ");
        std::fprintf(stderr, str, args);
        fprintf(stderr, ".\n");
        va_end(args);
}

class Token
{
public:
        Token() = default;
        Token(std::string word) : word(word), is_error(false)
        {}

        virtual ~Token() = default;

        std::string word;
        bool is_error;

        enum token_type
        {
                NONE = 0,
                REGISTER,
                LABEL,
                DECIMAL_IMMEDIATE,
                HEXADECIMAL_IMMEDIATE,
                BINARY_IMMEDIATE,
                DIR_STRINGZ,
                DIR_FILL,
                DIR_BLKW,
                DIR_ORIG,
                DIR_END,
                OP_AND,
                OP_ADD,
                OP_BR,
                OP_JMP,
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
                STRING,
        };

        virtual token_type type()
        { return token_type::NONE; }

        virtual uint64_t requires()
        { return static_cast<uint64_t >(token_type::NONE); }

        virtual Token expected(std::string expects)
        {
                std::cerr << "ERROR: Expected " << expects << ". Found '" << word << "' instead." << std::endl;

                is_error = true;

                return *this;
        }

        virtual Token note(std::string)
        { return *this; }
};

class Directive : public Token
{
public:
        Directive() : Token()
        {}
};

class Immediate : public Token
{
public:
        Immediate() : Token() {}
        Immediate(std::string &word) : Token(word)
        {}
};

class Operator : public Token
{
public:
        Operator() : Token()
        {}
};

class Register : public Token
{
public:
        Register(char which) : Token(), reg(which)
        {}

        char reg;

        token_type type() override
        { return token_type::REGISTER; }

        uint64_t requires() override
        { return static_cast<uint64_t >(token_type::NONE); }
};

class Label : public Token
{
public:
        Label(std::string &name) : Token(), label(name)
        {}

        std::string label;

        token_type type() override
        { return token_type::LABEL; }

        uint64_t requires() override
        { return static_cast<uint64_t >(token_type::NONE); }
};

class Decimal : public Immediate
{
public:
        Decimal(uint16_t value) : Immediate(), immediate(value)
        {}

        uint16_t immediate;

        Token::token_type type() override
        { return Token::token_type::DECIMAL_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(uint16_t value) : Immediate(), immediate(value)
        {}

        uint16_t immediate;

        Token::token_type type() override
        { return Token::token_type::HEXADECIMAL_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Binary : public Immediate
{
public:
        Binary(std::string &value) : Immediate(value)
        {
                if (value.length() > 16) {
                        immediate = 0;
                        expected("value up to 16 bits");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t >(std::bitset<16>(value).to_ulong());
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid binary value, but got");
                }
        }

        uint16_t immediate;

        Token::token_type type() override
        { return Token::token_type::BINARY_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }

        Token expected(std::string expects) override
        {
                std::cerr << "Error: Expected " << expects
                          << ". Found value with " << word.length() << " bits instead." << std::endl;

                is_error = true;

                return *this;
        }
};

class Stringz : public Directive
{
public:
        Stringz() : Directive()
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_STRINGZ; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::STRING); }
};

class Blkw : public Directive
{
public:
        Blkw() : Directive()
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_BLKW; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(Token::token_type::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(Token::token_type::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(Token::token_type::HEXADECIMAL_IMMEDIATE);
        }
};

class Fill : public Token
{
public:
        Fill() : Token()
        {}

        token_type type() override
        { return token_type::DIR_FILL; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(token_type::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(token_type::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(token_type::HEXADECIMAL_IMMEDIATE);
        }
};

class Orig : public Token
{
public:
        Orig() : Token()
        {}

        token_type type() override
        { return token_type::DIR_ORIG; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(token_type::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(token_type::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(token_type::HEXADECIMAL_IMMEDIATE);
        }
};

class End : public Token
{
public:
        End() : Token()
        {}

        token_type type() override
        { return token_type::DIR_END; }

        uint64_t requires() override
        { return static_cast<uint64_t >(token_type::NONE); }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
