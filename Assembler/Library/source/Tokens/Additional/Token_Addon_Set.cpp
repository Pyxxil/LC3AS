#include "Tokens/Additional/Token_Addon_Set.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Token_Register.hpp"

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

std::int32_t Set::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           std::uint16_t program_counter)
{
        if (!is_valid) {
                return -1;
        }

        const std::shared_ptr<Register>  reg    = std::static_pointer_cast<Register>(tokens.at(1));
        const std::shared_ptr<Immediate> offset = std::static_pointer_cast<Immediate>(tokens.at(2));

        if (offset->value > -16 && offset->value < 15) {
                std::vector<std::shared_ptr<Token>> vec = { _and, reg, reg, decimal_zero };
                _and->assemble(vec, symbols, program_counter);

                vec = { add, reg, reg, offset };
                add->assemble(vec, symbols, program_counter);

                assembled = _and->assembled;
                assembled.emplace_back(add->assembled.front());
        } else {
                std::vector<std::shared_ptr<Token>> vec = { br, decimal_one };
                br->assemble(vec, symbols, program_counter);

                vec = { fill, offset };
                fill->assemble(vec, symbols, program_counter);

                vec = { ld, reg, decimal_negative_two };
                ld->assemble(vec, symbols, program_counter);

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

std::string Set::disassemble(std::uint16_t &program_counter,
                             const std::string &symbol, int width) const
{

        std::stringstream stream;

        if (!_and->assembled.empty()) {
                stream << _and->disassemble(program_counter, symbol, width);
                stream << add->disassemble(program_counter, " ", width);
        } else {
                stream << br->disassemble(program_counter, symbol, width);
                stream << fill->disassemble(program_counter, " ", width);
                stream << ld->disassemble(program_counter, " ", width);
        }

        return stream.str();
}

Token::token_type Set::type() const
{
        return Token::ADDON_SET;
}
