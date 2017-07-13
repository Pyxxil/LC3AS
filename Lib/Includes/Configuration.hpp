#ifndef LC3_ASSEMBLER_CONFIGURATION_HPP
#define LC3_ASSEMBLER_CONFIGURATION_HPP

#include <string>

namespace Config
{
        enum CONFIG_OPTIONS
        {
                WARN_AS_ERROR     = 0x01,
                STOP_ON_FIRST_ERR = 0x02,
                VERBOSE           = 0x04,
        };

        extern std::size_t configuration;

        inline bool is_set(std::size_t option)
        {
                return !!(configuration & option);
        }

        inline void set(std::size_t option)
        {
                configuration |= option;
        }

        void set(std::size_t option);
        void add_search_directory(const std::string &directory);
        void add_search_directory(const std::string &&directory, const std::string &&name);

        // TODO: Make this return some sort of context, e.g. File that includes it
        std::string find_path(const std::string &path);
}

#endif
