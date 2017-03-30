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
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);

        if (word.at(0) == '.') {
                if (!word.compare(".ORIG")) {
                        // Return a Orig_Directive token
                        return Orig();
                } else if (!word.compare(".END")) {
                        return End();
                } else if (!word.compare(".FILL")) {
                        return Fill();
                } else if (!word.compare(".BLKW")) {
                        return Blkw();
                } else if (!word.compare(".STRINGZ")) {
                        return Stringz();
                }
                std::cout << "HERE IT IS: " << word << std::endl;
                return Token(word).expected("one of .FILL, .ORIG, .END, .STRINGZ, .BLKW.");
        } else {
                if (word.compare("ADD")) {

                } else if (word.compare("AND")) {

                } else if (word.compare("")) {

                }
        }

        return Token();
}
