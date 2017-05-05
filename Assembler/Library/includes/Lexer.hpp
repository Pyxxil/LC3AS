#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "Token.hpp"
#include "Logging/Logger.hpp"

class Assembler;

class Lexer
{
public:
        Lexer(const std::string &t_file_name);

        std::size_t parse_into(std::vector<std::vector<std::shared_ptr<Token>>> &into);

private:
        std::string m_file_name;
        Logger      m_logger;

        std::size_t m_error_count = 0;

        void populate_tokens(std::vector<std::vector<std::shared_ptr<Token>>> &into);
        void addToken(std::string &token, std::vector<std::shared_ptr<Token>> &to, int line_number);
        void tokenizeLine(const std::string &line, int line_number, std::vector<std::shared_ptr<Token>> &into);
        std::shared_ptr<Token> tokenize(std::string &word, int line_number);
};

#endif // LEXER_HPP
