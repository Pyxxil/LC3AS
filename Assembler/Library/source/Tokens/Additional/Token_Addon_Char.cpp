#include "Tokens/Additional/Token_Addon_Char.hpp"

Character::Character()
        : Immediate()
{

}

Character::Character(std::string &character, int line_number)
        : Immediate(character, line_number)
{
        if (character.length() > 1) {
                if (character.length() == 2 && character.at(0) == '\\') {
                        switch (character.at(1)) {
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
        } else if (!character.length()) {
                immediate = 0;
        } else {
                immediate = static_cast<std::int16_t>(character.at(0));
        }
}
