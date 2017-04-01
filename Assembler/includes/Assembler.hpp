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
        std::vector<std::vector<Token>> tokenizeFile(std::string &fileName);
        std::vector<Token> tokenizeLine(std::string &line, int line_number = 0); // This should return a vector

        Token tokenize(std::string &word, int line_number);
        bool validate(std::vector<Token> &tokens);
        std::map<Token, uint16_t> &generate_symbols(std::vector<Token, std::allocator<Token>> &tokens);

private:
        std::map<Token, uint16_t> symbols;
        void addToken(std::string &token, std::vector<Token> &tokens, int line_number);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
