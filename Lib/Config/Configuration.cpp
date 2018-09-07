#include "Configuration.hpp"

#if !defined(__clang__) || !defined(__APPLE__)
// Clang (on OSX anyways) won't work with this
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <boost/filesystem.hpp>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
namespace fs = boost::filesystem;
#endif

#include "Diagnostics.hpp"
#include "Extra.hpp"

struct Directory {
  // TODO: Probably move this somewhere else ... Maybe the Lexer?
  explicit Directory(const std::string &_directory,
                     std::string _name = std::string())
      : directory(_directory), name(std::move(_name))
  //    , exists(fs::exists(directory))
  //    , is_directory(fs::is_directory(directory))
  {}

  bool contains(const fs::path &path) {
    (void)path;

    if (!(exists && is_directory)) {
      if (Config::is_set(Config::VERBOSE)) {
        /*
        std::stringstream ss;
        ss << "Ignoring invalid directory \"" << name << '"';
        Console::write_line(ss.str());
        */
      }
      return false;
    }

    if (Config::is_set(Config::VERBOSE)) {
      /*
      std::stringstream ss;
      ss << "Searching \"" << name << '"';
      Console::write_line(ss.str());
      */
    }

    return false;
  }

  fs::path directory;
  std::string name;

  bool exists;
  bool is_directory;
};

static std::vector<Directory> search_directories;

size_t Config::configuration{0};

void Config::add_search_directory(const std::string &directory) {
  (void)directory;
  //  search_directories.emplace_back(Directory(directory));
}

void Config::add_search_directory(const std::string &&directory,
                                  const std::string &&name) {
  (void)directory;
  (void)name;
  //  search_directories.emplace_back(Directory(directory, name));
}

std::string Config::find_path(const std::string &path) {
  // TODO: This should all be done in a different file (Config is the wrong
  // place).
  const fs::path p{path};

  if (!fs::is_regular_file(p)) {
    return {};
  }

  if (p.is_absolute() && fs::exists(p)) {
    return path;
  }

  if (p.is_relative() && fs::exists(p)) {
    const fs::path abs_path = fs::absolute(p);
    return abs_path.string();
  }

  for (auto &&dir : search_directories) {
    if (dir.contains(p)) {
      const fs::path s{fs::absolute(path)};
      return s.string();
    }
  }

  return {};
}
