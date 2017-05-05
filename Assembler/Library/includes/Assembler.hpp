#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Tokens/Token_Label.hpp"
#include "Logging/Logger.hpp"
#include "Symbol.hpp"

class Assembler
{
public:
        Assembler();
        Assembler(int argument_count, char **arguments);
        Assembler(std::string &file);
        Assembler(std::string &&file);

        ~Assembler() = default;

        bool assemble();
        bool assemble(std::string &fileName);
        bool assemble(std::string &&fileName)
        {
                return assemble(fileName);
        }

        void write(std::string &prefix);
        void write(std::string &&prefix)
        {
                write(prefix);
        }

        std::uint16_t internal_program_counter   = 0;
        std::uint16_t file_memory_origin_address = 0;

        bool origin_seen = false;
        bool end_seen    = false;

        std::map<std::string, Symbol> symbols;

        std::string disassemble(std::uint16_t instruction, std::uint16_t pc);

        int longest_symbol_length = 20;

private:
        std::vector<std::string> files_to_assemble;

        std::vector<std::uint16_t> as_assembled;

        std::size_t error_count = 0;

        Logger m_logger;

        std::vector<std::vector<std::shared_ptr<Token>>> tokens;

        void do_first_pass();
        void do_second_pass();
        void reset();
        void generate_machine_code();

        bool do_write = true;

        bool quiet         = false;
        int  warning_level = Logger::WARNING_TYPE::ALL;
        void change_warning_level(std::string &warning);
};

#endif // ASSEMBLER_HPP
