#ifndef LC3_ASSEMBLER_ASSEMBLER_HPP
#define LC3_ASSEMBLER_ASSEMBLER_HPP

#include <string>

namespace Assembler
{
        class Assembler {
        public:
                Assembler();

                // TODO: Options:
                //      --include/-I[=]Directory: Include the directory in search paths for .INCLUDE directives
                //      --stop/-s:                Stop on the first error
                //      --error/-e:               Treat warnings as errors
        };

        extern Assembler *assembler;

        extern std::string last_error;

        bool init();
        void de_init();
};

#endif
