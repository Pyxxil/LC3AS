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
        };

        extern Assembler *assembler;

        extern std::string last_error;

        bool init();
        void de_init();

        extern std::vector<std::string> lines;
};

#endif
