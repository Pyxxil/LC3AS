#ifndef LC3_SIMULATOR_ASSEMBLER_HPP
#define LC3_SIMULATOR_ASSEMBLER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Tokens/Token_Label.hpp"

class Assembler
{
public:
        Assembler();
        ~Assembler() = default;
        std::vector<std::vector<std::shared_ptr<Token>>> &tokenizeFile(std::string &fileName);
        std::vector<std::shared_ptr<Token>> tokenizeLine(std::string &line, int line_number = 0);

        std::shared_ptr<Token> tokenize(std::string &word, int line_number);
        void assemble();
        std::vector<uint16_t> &generate_machine_code();
        void write(std::string &prefix);

        std::size_t   error_count                = 0;
        std::uint16_t internal_program_counter   = 0;
        std::uint16_t file_memory_origin_address = 0;

        bool origin_seen;
        bool end_seen;

        std::map<std::uint16_t, std::shared_ptr<Label>> symbols;

private:
        std::vector<std::uint16_t> as_assembled;

        std::vector<std::vector<std::shared_ptr<Token>>> tokens;
        void addToken(std::string &token, std::vector<std::shared_ptr<Token>> &toks, int line_number);

        void do_first_pass();
        void do_second_pass();
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
