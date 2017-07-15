#ifndef PROJECT_PARSER_HPP
#define PROJECT_PARSER_HPP

#include <string>
#include <map>

#include "Tokens/Token.hpp"
#include "Tokens/Symbol.hpp"

class Parser
{
public:
        Parser(const std::string &t_file)
                : file(t_file)
        { }

        int parse();

        inline uint16_t starting_address() const
        {
                return program_counter;
        }

private:
        std::string file;
        bool origin_seen = false;
        bool end_seen    = false;
        uint16_t internal_program_counter = 0;
        uint16_t program_counter;

        int longest_symbol_length = 20;
        std::map<std::string, Symbol> symbols;
        std::vector<std::vector<std::shared_ptr<Token>>> tokens;

        void do_first_pass();
        void do_second_pass();
};

#endif
