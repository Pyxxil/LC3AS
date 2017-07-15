#ifndef PROJECT_LEXER_HPP
#define PROJECT_LEXER_HPP

#include <fstream>
#include <memory>

#include "Tokens/Token.hpp"

class Lexer
{
public:
        Lexer() = delete;
        Lexer(const std::string &t_file)
                : file_name(t_file), file(t_file), parent(nullptr)
        { }
        Lexer(Lexer *const t_parent, const std::string &t_file)
                : file_name(t_file), file(t_file), parent(t_parent)
        { }
        Lexer(Lexer *const other)
                : file_name(other->file_name), file(std::move(other->file)), symbols(other->symbols)
                  , parent(other)
        { }

        void lex(std::vector<std::vector<std::shared_ptr<Token>>> &t_tokens, std::map<std::string, Symbol> &t_symbols);
        std::shared_ptr<Token> tokenize(std::string &word, size_t line_number, size_t column);
        void tokenizeLine(const std::string &line, size_t line_number, std::vector<std::shared_ptr<Token>> &into);
        void addToken(std::string &token,
                      std::vector<std::shared_ptr<Token>> &to,
                      size_t line_number,
                      size_t col);

        inline const auto &get_tokens() const
        {
                return tokens;
        }

        inline const auto &get_symbols() const
        {
                return symbols;
        }

private:
        std::string                   file_name;
        std::ifstream                 file;
        std::map<std::string, Symbol> symbols;
        Lexer *const parent;
        std::vector<std::vector<std::shared_ptr<Token>>> tokens;
};

#endif
