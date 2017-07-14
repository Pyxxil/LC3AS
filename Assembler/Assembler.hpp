#ifndef LC3_ASSEMBLER_ASSEMBLER_HPP
#define LC3_ASSEMBLER_ASSEMBLER_HPP

#include <vector>
#include <string>

namespace Assembler
{
        class Assembler
        {
        public:
                Assembler();

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
                //                                      TODO: Decide if this requires differing levels (e.g. -vvv or --verbose-level=3)
                //                                      TODO:   If so, add enum options for config (VERBOSE_LEVEL_ONE, VERBOSE_LEVEL_TWO, etc.)
                //                                      TODO: Decide if this relates to diagnostics too
                bool configure(int argc, char **args);

                void assemble();

        private:
                std::vector<std::string> files_to_assemble;
                std::size_t errors;
        };

        int assemble(int argc, char **args);
}

#endif
