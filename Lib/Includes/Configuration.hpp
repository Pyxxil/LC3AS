#ifndef LC3_ASSEMBLER_CONFIGURATION_HPP
#define LC3_ASSEMBLER_CONFIGURATION_HPP

#include <string>

namespace Config {
enum CONFIG_OPTIONS
{
  WARN_AS_ERROR = 0x01,
  STOP_ON_FIRST_ERR = 0x02,
  VERBOSE = 0x04,
  BE_QUIET = 0x08,
  KEEP_GOING = 0x10,
  NO_COLOUR = 0x20,
};

extern size_t configuration;

bool
is_set(size_t option);

void
set(size_t option);

void
add_search_directory(const std::string& directory);
void
add_search_directory(const std::string&& directory, const std::string&& name);

// TODO: Make this return some sort of context, e.g. File that includes it
std::string
find_path(const std::string& path);
} // namespace Config

#endif
