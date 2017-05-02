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
        Assembler(int argument_count, char **arguments);
        Assembler(std::string &file);
        Assembler(std::string &&file);

        ~Assembler() = default;

        std::vector<std::vector<std::shared_ptr<Token>>> &tokenizeFile(std::string &fileName);
        std::vector<std::vector<std::shared_ptr<Token>>> &tokenizeFile(std::string &&fileName)
        {
                return tokenizeFile(fileName);
        }

        std::vector<std::shared_ptr<Token>> tokenizeLine(std::string &line, int line_number = 0);

        std::shared_ptr<Token> tokenize(std::string &word, int line_number);
        std::vector<std::uint16_t> generate_machine_code();

        bool assemble();
        bool assemble(std::string &fileName);
        bool assemble(std::string &&fileName)
        {
                return assemble(fileName);
        }

        std::string check_for_symbol_match(const std::string &symbol) const;

        void write(std::string &prefix);
        void write(std::string &&prefix)
        {
                write(prefix);
        }

        std::uint16_t internal_program_counter   = 0;
        std::uint16_t file_memory_origin_address = 0;

        bool origin_seen = false;
        bool end_seen = false;

        std::map<std::string, std::shared_ptr<Label>> symbols;

        std::string disassemble(std::uint16_t instruction, std::uint16_t pc);

        enum WARNING_TYPE
        {
                NONE                 = 0,
                SYNTAX               = 1 << 0,
                IGNORED              = 1 << 1,
                MULTIPLE_DEFINITIONS = 1 << 2,
                LOGIC                = 1 << 3,
                ALL                  = SYNTAX | IGNORED | MULTIPLE_DEFINITIONS | LOGIC,
        };

        enum LOGGING_TYPE
        {
                MESSAGE = 0,
                ERROR   = 1,
                WARNING = 2,
        };

        int longest_symbol_length = 20;

private:
        std::vector<std::string> files_to_assemble;

        std::vector<std::uint16_t> as_assembled;

        std::size_t error_count = 0;

        std::vector<std::vector<std::shared_ptr<Token>>> tokens;
        void addToken(std::string &token, std::vector<std::shared_ptr<Token>> &t_tokens, int line_number);

        void do_first_pass();
        void do_second_pass();
        void reset();

        bool quiet = false;
        bool we_should_be_quiet()
        {
                return quiet;
        }

        void LOG(LOGGING_TYPE level, std::string &&message);
        void WARN(WARNING_TYPE level, int line_number, std::string &&warning);
        void ERR(int line_number, std::string &&error);

        int warning_level = ALL;
        void change_warning_level(std::string &warning);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
