#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "Token.hpp"
#include "Logging/Logger.hpp"

class Assembler;

class Tokenizer
{
public:
        Tokenizer() : m_file_name() {}
        Tokenizer(const std::string &t_file_name);
        ~Tokenizer();

        std::size_t parse_into(std::vector<std::vector<std::shared_ptr<Token>>> &into);
        void tokenizeLine(const std::string &line, int line_number, std::vector<std::shared_ptr<Token>> &into);
        void tokenizeLine(const std::string &&line, int line_number, std::vector<std::shared_ptr<Token>> &into)
        { tokenizeLine(line, line_number, into); }

        std::size_t error_count() const { return m_error_count; }
        std::size_t warn_count() const { return m_warn_count; }

private:
        std::string m_file_name;

        std::size_t m_error_count = 0;
        std::size_t m_warn_count = 0;

        void populate_tokens(std::vector<std::vector<std::shared_ptr<Token>>> &into);
        void addToken(std::string &token, std::vector<std::shared_ptr<Token>> &to, int line_number);
        std::shared_ptr<Token> tokenize(std::string &word, int line_number);

        static std::vector<std::string> open_files;
};

#endif // LEXER_HPP
