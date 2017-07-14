#include "Tokens/Token_Addon_Char.hpp"

Character::Character()
        : Immediate()
{

}

Character::Character(std::string &character, std::string &t_file, int line_number)
        : Immediate(character, character, t_file, line_number)
{
        if (character.length() > 1) {
                if (character.length() == 2 && character.at(0) == '\\') {
                        switch (character.at(1)) {
                        case '\\':
                                value = '\\';
                                return;
                        case 'n':
                                value = '\n';
                                return;
                        case 't':
                                value = '\t';
                                return;
                        case '\'':
                                value = '\'';
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
                std::cerr << "Invalid character literal: '" << token << "'.\n";
        } else if (!character.length()) {
                value = 0;
        } else {
                value = static_cast<std::int16_t>(character.at(0));
        }
}
