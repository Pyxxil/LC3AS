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
        Token(std::string word) : word(word)
        {}

        virtual ~Token() = default;

        std::string word;

        enum class TokenType : uint64_t
        {
                NONE = 0,
                REGISTER = 1ull << 0,
                LABEL = 1ull << 1,
                DECIMAL_IMMEDIATE = 1ull << 2,
                HEXADECIMAL_IMMEDIATE = 1ull << 3,
                BINARY_IMMEDIATE = 1ull << 4,
                DIR_STRINGZ = 1ull << 5,
                DIR_FILL = 1ull << 6,
                DIR_BLKW = 1ull << 7,
                DIR_ORIG = 1ull << 8,
                DIR_END = 1ull << 9,
                OP_AND = 1ull << 10,
                OP_ADD = 1ull << 11,
                OP_BR = 1ull << 12,
                OP_JMP = 1ull << 13,
                OP_JSR = 1ull << 14,
                OP_JSRR = 1ull << 15,
                OP_RTI = 1ull << 16,
                OP_NOT = 1ull << 17,
                OP_LDI = 1ull << 18,
                OP_LEA = 1ull << 19,
                OP_LDR = 1ull << 20,
                OP_LD = 1ull << 21,
                OP_ST = 1ull << 22,
                OP_STI = 1ull << 23,
                OP_STR = 1ull << 24,
                OP_TRAP = 1ull << 25,
                TRAP_PUTS = 1ull << 26,
                TRAP_GETC = 1ull << 27,
                TRAP_IN = 1ull << 28,
                TRAP_OUT = 1ull << 29,
                TRAP_PUTSP = 1ull << 30,
                TRAP_HALT = 1ull << 31,
                STRING = 1ull << 32,
        };

        virtual TokenType type()
        { return TokenType::NONE; }

        virtual uint64_t requires()
        { return static_cast<uint64_t >(TokenType::NONE); }

        virtual Token expected(std::string expects)
        {
                std::cerr << "ERROR: Expected " << expects << " Found '" << word << "' instead." << std::endl;
                return *this;
        }
};

class Register : public Token
{
public:
        Register(char which) : Token(), reg(which)
        {}

        char reg;

        TokenType type() override
        { return TokenType::REGISTER; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

class Label : public Token
{
public:
        Label(std::string &name) : Token(), label(name)
        {}

        std::string label;

        TokenType type() override
        { return TokenType::LABEL; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

class Decimal : public Token
{
public:
        Decimal(uint16_t value) : Token(), immediate(value)
        {}

        uint16_t immediate;

        TokenType type() override
        { return TokenType::DECIMAL_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

class Hexadecimal : public Token
{
public:
        Hexadecimal(uint16_t value) : Token(), immediate(value)
        {}

        uint16_t immediate;

        TokenType type() override
        { return TokenType::HEXADECIMAL_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

class Binary : public Token
{
public:
        Binary(uint16_t value) : Token(), immediate(value)
        {}

        uint16_t immediate;

        TokenType type() override
        { return TokenType::BINARY_IMMEDIATE; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

class Stringz : public Token
{
public:
        Stringz() : Token() {}

        TokenType type() override
        { return TokenType::DIR_STRINGZ; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::STRING); }
};

class Blkw : public Token
{
public:
        Blkw() : Token() {}

        TokenType type() override
        { return TokenType::DIR_BLKW; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::HEXADECIMAL_IMMEDIATE); }
};

class Fill : public Token
{
public:
        Fill() : Token() {}

        TokenType type() override
        { return TokenType::DIR_FILL; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::HEXADECIMAL_IMMEDIATE); }
};

class Orig : public Token
{
public:
        Orig() : Token() {}

        TokenType type() override
        { return TokenType::DIR_ORIG; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::BINARY_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::DECIMAL_IMMEDIATE) |
                        static_cast<uint64_t >(TokenType::HEXADECIMAL_IMMEDIATE); }
};

class End : public Token
{
public:
        End() : Token() {}

        TokenType type() override
        { return TokenType::DIR_END; }

        uint64_t requires() override
        { return static_cast<uint64_t >(TokenType::NONE); }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
