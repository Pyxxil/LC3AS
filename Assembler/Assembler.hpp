#ifndef LC3_ASSEMBLER_ASSEMBLER_HPP
#define LC3_ASSEMBLER_ASSEMBLER_HPP

#include <vector>
#include <string>
#include <map>

#include "Tokens/Tokens.hpp"

namespace Assembler
{
class Assembler
{
public:
    Assembler()
        : symbol_padding(0)
    {}

    // TODO: Options:
    //      --include/-I[=]Directory:
    //              Default: Current Working Directory
    //                                      Include the directory in search paths for .INCLUDE directives
    //      --stop/-s:
    //              Default: false          Stop on the first error
    //      --error/-e:
    //              Default: false          Report warnings as errors
    //      --warn=[all,syntax,logic,none]
    //              Default = all           Enable/Disable warnings
    //      --verbose/-v                    Enable Verbose logging
    //                                      TODO: Decide if this requires differing levels (e.g. -vvv or
    //                                            --verbose-level=3)
    //                                      TODO:   If so, add enum options for config (VERBOSE_LEVEL_ONE,
    //                                              VERBOSE_LEVEL_TWO, etc.)
    //                                      TODO: Decide if this relates to diagnostics too
    bool configure(int argc, char **args);

    void assemble();

private:
    void write(const std::string &file);
    void generate_machine_code();
    void check_and_mark_warnings(const std::vector<std::shared_ptr<Token>> &tokenized_line,
                                 uint16_t assembled_line,
                                 size_t i);

    std::vector<std::string> files_to_assemble;
    std::vector<std::vector<std::shared_ptr<Token>>> tokens;
    std::map<std::string, Symbol> symbols;
    std::vector<uint16_t> assembled;
    int symbol_padding{};
};

int assemble(int argc, char **args);
}

#endif
