#ifndef LC3_SIMULATOR_ASSEMBLER_HPP
#define LC3_SIMULATOR_ASSEMBLER_HPP

#include <string>
#include <vector>
#include <map>

#include "Token.hpp"

class Assembler
{
public:
        Assembler();
        std::vector<std::vector<std::unique_ptr<Token>>> tokenizeFile(std::string &fileName);
        std::vector<std::unique_ptr<Token>> tokenizeLine(std::string &line, int line_number = 0); // This should return a vector

        std::unique_ptr<Token> tokenize(std::string &word, int line_number);
        bool validate(std::vector<Token> &tokens);
        std::map<Token, std::uint16_t> &generate_symbols(std::vector<std::vector<std::unique_ptr<Token>>> &tokens);

        std::size_t errors;
        std::uint16_t file_memory_orig_addr;

private:
        std::map<Token, std::uint16_t> symbols;
        void addToken(std::string &token, std::vector<std::unique_ptr<Token>> &tokens, int line_number);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
