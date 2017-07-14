#ifndef PROJECT_LEXER_HPP
#define PROJECT_LEXER_HPP

#include <fstream>
#include <memory>

#include "Lib/Includes/Tokens/Token.hpp"

class Lexer
{
public:
        Lexer() = delete;
        Lexer(const std::string &t_file)
                : file_name(t_file), file(t_file)
        { }
        Lexer(Lexer &t_parent, const std::string &t_file)
                : file_name(t_file), file(t_file), parent(std::make_unique<Lexer>(t_parent))
        { }
        Lexer(Lexer &other)
                : file_name(other.file_name), file(std::move(other.file)), symbols(other.symbols), parent(std::move(other.parent))
        { }
        Lexer(Lexer &&other)
                : file_name(other.file_name), file(std::move(other.file)), symbols(other.symbols), parent(std::move(other.parent))
        { }

        void lex();
        std::shared_ptr<Token> tokenize(std::string &word, int line_number);
        void tokenizeLine(const std::string &line, int line_number, std::vector<std::shared_ptr<Token>> &into);
        void addToken(std::string &token, std::vector<std::shared_ptr<Token>> &to, int line_number);

private:
        std::string                         file_name;
        std::ifstream                       file;
        std::map<std::string, Symbol>       symbols;
        std::unique_ptr<Lexer>              parent;
        std::vector<std::vector<std::shared_ptr<Token>>> tokens;
};

#endif
