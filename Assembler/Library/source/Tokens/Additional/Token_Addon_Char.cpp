#include "Tokens/Additional/Token_Addon_Char.hpp"

Character::Character()
        : Immediate()
{

}

Character::Character(std::string &word, int line_number)
        : Immediate(word, line_number)
{
        if (word.length() > 1) {
                if (word.length() == 2 && word.at(0) == '\\') {
                        switch (word.at(1)) {
                        case '\\':
                                immediate = '\\';
                                return;
                        case 'n':
                                immediate = '\n';
                                return;
                        case 't':
                                immediate = '\t';
                                return;
                        default:
                                break;
                        }
                }

                is_valid = false;
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Invalid character literal: '" << word << "'.\n";
        } else if (!word.length()) {
                immediate = 0;
        } else {
                immediate = static_cast<std::int16_t>(word.at(0));
        }
}

void Character::unterminated()
{
        is_valid = false;
        std::cerr << "ERROR: ";
        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }
        std::cerr << "Unterminated character literal.\n";
}

