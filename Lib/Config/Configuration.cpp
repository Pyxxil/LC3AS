#include "Configuration.hpp"

#include <vector>

static std::size_t configuration = 0;
static std::vector<std::string> search_directories;

bool Config::config_on(std::size_t option)
{
        return !!(configuration & option);
}

void Config::config_set(std::size_t option)
{
        configuration |= option;
}

void Config::config_add_search_directory(const std::string &directory)
{
        search_directories.emplace_back(directory);
}
