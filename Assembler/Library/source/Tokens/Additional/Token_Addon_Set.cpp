#include "Tokens/Additional/Token_Addon_Set.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Set::Set()
        : Directive()
{
        _and                 = std::make_shared<And>();
        add                  = std::make_shared<Add>();
        br                   = std::make_shared<Br>(true, true, true);
        ld                   = std::make_shared<Ld>();
        fill                 = std::make_shared<Fill>();
        decimal_one          = std::make_shared<Decimal>("#1");
        decimal_negative_two = std::make_shared<Decimal>("#-2");
        decimal_zero         = std::make_shared<Decimal>("#0");
}

Set::Set(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{
        _and                 = std::make_shared<And>();
        add                  = std::make_shared<Add>();
        br                   = std::make_shared<Br>(true, true, true);
        ld                   = std::make_shared<Ld>();
        fill                 = std::make_shared<Fill>();
        decimal_one          = std::make_shared<Decimal>("#1");
        decimal_negative_two = std::make_shared<Decimal>("#-2");
        decimal_zero         = std::make_shared<Decimal>("#0");

        _and->at_line = add->at_line = br->at_line = ld->at_line = fill->at_line = decimal_zero->at_line =
        decimal_one->at_line = decimal_negative_two->at_line = line_number;
}

std::int32_t Set::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        const std::shared_ptr<Register>  reg    = std::static_pointer_cast<Register>(tokens.at(1));
        const std::shared_ptr<Immediate> offset = std::static_pointer_cast<Immediate>(tokens.at(2));

        if (offset->value > -16 && offset->value < 15) {
                std::vector<std::shared_ptr<Token>> vec = {_and, reg, reg, decimal_zero};
                _and->assemble(vec, assembler);

                vec = {add, reg, reg, offset};
                add->assemble(vec, assembler);

                assembled = _and->assembled;
                assembled.emplace_back(add->assembled.front());
        } else {
                std::vector<std::shared_ptr<Token>> vec = {br, decimal_one};
                br->assemble(vec, assembler);

                vec = {fill, offset};
                fill->assemble(vec, assembler);

                vec = {ld, reg, decimal_negative_two};
                ld->assemble(vec, assembler);

                assembled = br->assembled;
                assembled.emplace_back(fill->assembled.front());
                assembled.emplace_back(ld->assembled.front());
        }

        return static_cast<std::int32_t>(assembled.size());
}

bool Set::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
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
                tokens.at(2)->expected("value value");
                return (is_valid = false);
        } else if (!tokens.at(2)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Set::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (!is_valid) {
                return -1;
        }

        const std::int16_t value = std::static_pointer_cast<Immediate>(tokens.at(2))->value;

        if (value > 15 || value < -16) {
                return 3;
        } else {
                return 2;
        }
}

std::string Set::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                             std::uint16_t &program_counter,
                             const std::string &symbol,
                             const Assembler &assembler) const
{
        const std::shared_ptr<Register>  reg    = std::static_pointer_cast<Register>(tokens.at(1));
        const std::shared_ptr<Immediate> offset = std::static_pointer_cast<Immediate>(tokens.at(2));

        std::stringstream stream;

        if (offset->value > -16 && offset->value < 15) {
                std::vector<std::shared_ptr<Token>> vec = {_and, reg, reg, decimal_zero};
                stream << _and->disassemble(vec, program_counter, symbol, assembler);

                vec = {add, reg, reg, offset};
                stream << add->disassemble(vec, program_counter, " ", assembler);
        } else {
                std::vector<std::shared_ptr<Token>> vec = {br, decimal_one};
                stream << br->disassemble(vec, program_counter, symbol, assembler);

                vec = {fill, offset};
                stream << fill->disassemble(vec, program_counter, " ", assembler);

                vec = {ld, reg, decimal_negative_two};
                stream << ld->disassemble(vec, program_counter, " ", assembler);
        }

        return stream.str();
}

Token::token_type Set::type() const
{
        return Token::ADDON_SET;
}
