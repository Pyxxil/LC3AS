#include "Tokens/Additional/Token_Addon_Lshift.hpp"

#include <sstream>

#include "Tokens/Token_Immediate.hpp"

Lshift::Lshift()
{

}

Lshift::Lshift(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{
        add = std::make_shared<Add>();
        add->at_line = line_number;
}

std::int32_t Lshift::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        std::vector<std::shared_ptr<Token>> vec = { add, tokens.at(1), tokens.at(1), tokens.at(1) };

        add->assemble(vec, assembler);

        std::uint16_t machine_instruction = add->assembled.front();

        for (int shift = 0; shift < std::static_pointer_cast<Immediate>(tokens.at(2))->value; ++shift) {
                assembled.push_back(machine_instruction);
        }

        return static_cast<std::int32_t>(assembled.size());
}

bool Lshift::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        if (tokens.at(2)->type() != Token::IMMEDIATE) {
                tokens.at(2)->expected("immediate value");
                return (is_valid = false);
        } else if (!tokens.at(2)->is_valid) {
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(2))->value > 15 ||
            std::static_pointer_cast<Immediate>(tokens.at(2))->value < 1) {
                tokens.at(2)->expected("4 bit unsigned value between 1 and 15 (inclusive)");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Lshift::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (!is_valid) {
             return 0;
        }

        return std::static_pointer_cast<Immediate>(tokens.at(2))->value;
}

std::string Lshift::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                std::uint16_t &program_counter,
                                const std::string &symbol,
                                const Assembler &assembler) const
{
        std::stringstream stream;
        std::vector<std::shared_ptr<Token>> vec = { add, tokens.at(1), tokens.at(1), tokens.at(1) };

        stream << add->disassemble(vec, program_counter, symbol, assembler);
        const std::string &disassembled_without_symbol = add->disassemble(vec, program_counter, "", assembler);

        // The previous instruction will increment this. We want it to be it's original value,
        // as we increment it ourself in the loop below.
        --program_counter;

        for (int shift = 1; shift < std::static_pointer_cast<Immediate>(tokens.at(2))->value; ++shift) {
                stream << disassembled_without_symbol;
                ++program_counter;
        }

        return stream.str();
}

Token::token_type Lshift::type() const
{
        return Token::type();
}
