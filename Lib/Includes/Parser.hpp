#ifndef PROJECT_PARSER_HPP
#define PROJECT_PARSER_HPP

#include <string>
#include <map>
#include <utility>

#include "Tokens/Token.hpp"
#include "Tokens/Symbol.hpp"

class Parser
{
public:
    explicit Parser(std::string t_file)
        : file(std::move(t_file)), origin_seen(false), end_seen(false), internal_program_counter(0), program_counter(0),
          longest_symbol_length(20), symbols(), tokens()
    {}

    int parse();

    inline uint16_t starting_address() const
    {
        return program_counter;
    }

    inline const std::vector<std::vector<std::shared_ptr<Token>>> &parsed_tokens() const
    {
        return tokens;
    }

    inline const std::map<std::string, Symbol> &parsed_symbols() const
    {
        return symbols;
    }

    inline int symbol_padding() const
    {
        return longest_symbol_length;
    }

private:
    std::string file;

    bool origin_seen;
    bool end_seen;

    uint16_t internal_program_counter;
    uint16_t program_counter;

    int longest_symbol_length;

    std::map<std::string, Symbol> symbols;

    std::vector<std::vector<std::shared_ptr<Token>>> tokens;

    std::shared_ptr<Token> origin;
    std::shared_ptr<Token> end;

    void do_first_pass();
    void do_second_pass();
};

#endif
