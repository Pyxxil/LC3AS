#include "Configuration.hpp"

#if !defined(_WIN64)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <boost/filesystem.hpp>
#include <utility>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif

#include "Diagnostics.hpp"

struct Directory
{
        // TODO: Probably move this somewhere else ... Maybe the Lexer?
        explicit Directory(std::string _directory, std::string _name = "")
                : directory(std::move(_directory)), name(std::move(_name)), is_directory(false)
        { }

        std::string name;
	    std::string directory;
        bool is_directory;
};

static std::vector<Directory> search_directories;

size_t Config::configuration = 0;

void Config::add_search_directory(const std::string &directory)
{
        search_directories.emplace_back(Directory(directory));
}

void Config::add_search_directory(const std::string &&directory, const std::string &&name)
{
        search_directories.emplace_back(Directory(directory, name));
}

std::string Config::find_path(const std::string &path)
{
/*
        // TODO: This should all be done in a different file (Config is the wrong place).
        const boost::filesystem::path p { path };

        if (!boost::filesystem::is_regular_file(p)) {
                return std::string();
        }

        if (p.is_absolute() && boost::filesystem::exists(p)) {
                return path;
        }

        if (p.is_relative() && boost::filesystem::exists(p)) {
                const boost::filesystem::path abs_path = boost::filesystem::absolute(p);
                return abs_path.string();
        }

        for (auto &&dir : search_directories) {
                if (dir.contains(p)) {
                        const boost::filesystem::path s { boost::filesystem::absolute(path) };
                        return s.string();
                }
        }
*/
        return std::string();
}
