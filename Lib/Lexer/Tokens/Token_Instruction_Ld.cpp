#include "Tokens/Token_Instruction_Ld.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"

Ld::Ld(std::string &instruction,
       std::string &instruction_uppercase,
       std::string &t_file,
       size_t line_number,
       size_t column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{}

Ld::Ld(std::string &&instruction,
       std::string &&instruction_uppercase,
       std::string &t_file,
       size_t line_number,
       size_t column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{}

int32_t Ld::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                          const std::map<std::string, Symbol> &symbols,
                          uint16_t program_counter)
{
    if (!is_valid) {
        return 0;
    }

    int offset = 0;

    if (tokens.at(2)->type() == Token::LABEL) {
        if (0u == symbols.count(tokens.at(2)->token)) {
            std::static_pointer_cast<Label>(tokens.at(2))->not_found(symbols);
            return -1;
        }

        offset = static_cast<int>(symbols.find(tokens.at(2)->token)->second.address) -
            (static_cast<int>(program_counter) + 1);
    }
    else {
        offset = std::static_pointer_cast<Immediate>(tokens.at(2))->value;
    }

    if (offset > 255 || offset < -256) {
        tokens.at(2)->requires_too_many_bits(9, SIGNED, this, symbols);
        return -1;
    }

    provided = tokens.at(2);
    assembled.emplace_back(
        static_cast<uint16_t >(0x2000 |
            ((std::static_pointer_cast<Register>(tokens.at(1))->reg & 7) << 9) |
            (offset & 0x1FF))
    );

    return 1;
}

bool Ld::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
    if (tokens.size() != 3) {
        invalid_argument_count(tokens.size(), 2, tokens.back()->at_column + tokens.back()->token.length());
        return false;
    }

    if (tokens.at(1)->type() != Token::REGISTER) {
        tokens.at(1)->expected("register");
        return (is_valid = false);
    }

    if (tokens.at(2)->type() != Token::LABEL && tokens.at(2)->type() != Token::IMMEDIATE) {
        tokens.at(2)->expected("label or immediate value");
        return (is_valid = false);
    }

    if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
        return (is_valid = false);
    }

    return is_valid;
}

uint16_t Ld::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
    (void) tokens;
    return static_cast<uint16_t>(is_valid);
}

std::string Ld::disassemble(uint16_t &program_counter,
                            const std::string &symbol,
                            int width) const
{
    std::stringstream stream;
    stream
        // Address in memory
        << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
        // Hexadecimal representation of instruction
        << ' ' << std::hex << std::setfill('0') << std::setw(4) << assembled.front()
        // Binary representation of instruction
        << ' ' << std::bitset<16>(assembled.front())
        // Line the instruction is on
        << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
        // Label at the current address (if any)
        << ' ' << std::left << std::setfill(' ') << std::setw(width) << symbol
        // Instruction itself
        << " LD R" << ((assembled.front() & 0x0E00) >> 9 & 7) << ' ';

    ++program_counter;

    if (provided->type() == Token::LABEL) {
        stream << provided->token;
    }
    else {
        const auto offset = std::static_pointer_cast<Immediate>(provided)->value;
        stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (offset + program_counter);
    }

#ifdef INCLUDE_ADDONS
    stream << '\t' << file;
#endif
    stream << '\n';

    return stream.str();
}

Token::token_type Ld::type() const
{
    return Token::token_type::OP_LD;
}
