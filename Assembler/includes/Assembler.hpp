#ifndef LC3_SIMULATOR_ASSEMBLER_HPP
#define LC3_SIMULATOR_ASSEMBLER_HPP

#include <string>
#include <vector>
#include <map>

#include "Token.hpp"
#include "Token_Label.hpp"

#include <memory>

class Assembler
{
public:
        Assembler();
        ~Assembler();
        std::vector<std::vector<Token *>> &tokenizeFile(std::string &fileName);
        std::vector<Token *> tokenizeLine(std::string &line, int line_number = 0); // This should return a vector

        Token *tokenize(std::string &word, int line_number);
        void assemble();
        std::vector<std::uint16_t> assembled();

        std::size_t error_count;
        std::uint16_t internal_program_counter = 0;
        std::uint16_t file_memory_origin_address = 0;

private:
        std::map<std::uint16_t, Label *> symbols;
        std::vector<std::vector<Token *>> tokens;
        void addToken(std::string &token, std::vector<Token *> &toks, int line_number);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
