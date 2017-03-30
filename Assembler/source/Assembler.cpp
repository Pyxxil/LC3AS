#include "../includes/Assembler.hpp"

#include <fstream>
#include <iostream>

Assembler::Assembler()
{

}

void Assembler::parseFile(std::string &fileName)
{
        std::ifstream file(fileName);
        //file.open(fileName);

        if (!file.is_open()) {
                perror(PROJECT);
                exit(EXIT_FAILURE);
        }

        std::string line;

        while (std::getline(file, line)) {
                if (line.empty()) {
                        continue;
                }

                parseLine(line); // This should return a vector of instructions
        }
}

void Assembler::parseLine(std::string &line)
{
        char character;

        std::string current;

        std::cout << "Going in: " << line << std::endl;

        for (std::size_t index = 0; index < line.length(); /* Is this the place to update? */) {
                character = line.at(index);

                // Deal with strings here, it'll be much simpler.

                if (std::isspace(character)) {
                        //std::cout << "Found some whitespace, skipping and dumping contents" << std::endl;
                        // We don't care about space characters.
                        // However, it does mean we want to check what we just got.
                        while (line.length() > index + 1 && std::isspace(character)) {
                                character = line.at(++index);
                        }

                        if (!current.empty()) {
                                std::cout << current << std::endl;
                                encode(current);
                                current.erase();
                        }

                        //std::cout << "Char after whitespace is '" << character << "'" << std::endl;

                        // Later, change it to vector.push_back(encode(current));
                        // Then update the vector we want to return, and then continue.
                }

                if (character == ';') {
                        // We've hit a comment, so skip to the end of the line.
                        //std::cout << "Reached comment, so end of line" << std::endl;
                        break;
                } else if (character == '/') {
                        // '//' is a comment as well.
                        if (index + 1 > line.length() || line.at(index + 1) != '/') {
                                // It seems easiest to treat it as a comment anyways, as '/' can't be used for anything.
                                WARNING("Expected '//', but found '/'. Treating it as if it's '//' (i.e. comment)");
                        }

                        // std::cout << "Reached comment, so end of line" << std::endl;
                        break;
                }

                current += character;
                ++index;
        }

        //std::cout << "Reached end of line, dumping contents" << std::endl;
        if (!current.empty()) {
                std::cout << current << std::endl;
                encode(current);
                current.erase();
        }
}

Token Assembler::encode(std::string &word)
{
        std::string copy = word;
        std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

        switch (copy.at(0)) {
                // This is checked because a label can't contain '.'
        case '.':
                if (copy == ".ORIG") {
                        return Orig();
                } else if (copy == ".END") {
                        return End();
                } else if (copy == ".FILL") {
                        return Fill();
                } else if (copy == ".BLKW") {
                        return Blkw();
                } else if (copy == ".STRINGZ") {
                        return Stringz();
                }
                return Token(word).expected("one of .FILL, .ORIG, .END, .STRINGZ, .BLKW.");
        case '0':
                if (copy.at(1) == 'X') {

                }
                break;
        case '#': break;
        case '-': break;
        case 'B':
                if (copy.at(1) == '0' || copy.at(1) == '1') {
                        copy = word.substr(1);
                        return Binary(copy);
                }
                break;
        case 'X':
                // We basically just assume it's a hexadecimal number right?
                break;
        default:break;
        }

        if (copy == "ADD") {

        } else if (copy == "AND") {

        } else if (copy == "NOT") {

        } else if (copy == "JSR") {

        } else if (copy == "JSRR") {

        } else if (copy == "JMP") {

        } else if (copy == "ST") {

        } else if (copy == "STR") {

        } else if (copy == "STI") {

        } else if (copy == "LD") {

        } else if (copy == "LEA") {

        } else if (copy == "LDI") {

        } else if (copy == "LDR") {

        } else if (copy == "PUTS") {

        } else if (copy == "PUTSP") {

        } else if (copy == "HALT") {

        } else if (copy == "TRAP") {

        } else if (copy == "GETC") {

        } else if (copy == "OUT") {

        } else if (copy == "IN") {

        }

        return Label(word);
}
