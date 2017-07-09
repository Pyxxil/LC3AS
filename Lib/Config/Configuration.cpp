#include <cstddef>
#include "Configuration.hpp"

static std::size_t configuration = 0;

bool Config::config_on(std::size_t option)
{
        return !!(configuration & option);
}

void Config::config_set(std::size_t option)
{
        configuration |= option;
}