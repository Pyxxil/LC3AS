#ifndef LC3_ASSEMBLER_CONFIGURATION_HPP
#define LC3_ASSEMBLER_CONFIGURATION_HPP

namespace Config
{
        enum CONFIG_OPTIONS
        {
                WARN_AS_ERROR     = 0x01,
                STOP_ON_FIRST_ERR = 0x02,
        };

        bool config_on(std::size_t option);
        void config_set(std::size_t option);
}

#endif
