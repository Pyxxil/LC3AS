#include "Configuration.hpp"

#include <experimental/filesystem>

#include "Diagnostics.hpp"

struct Directory
{
        // TODO: Probably move this somewhere else ... Maybe the Lexer?
        explicit Directory(const std::string &_directory, std::string _name = "")
                : directory(_directory), name(std::move(_name)), exists(std::experimental::filesystem::exists(directory))
                  , is_directory(std::experimental::filesystem::is_directory(directory))
        { }

        bool contains(const std::experimental::filesystem::path &path)
        {
                (void) path;

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

        std::experimental::filesystem::path directory;
        std::string             name;

        bool exists;
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
        // TODO: This should all be done in a different file (Config is the wrong place).
        const std::experimental::filesystem::path p { path };

        if (!std::experimental::filesystem::is_regular_file(p)) {
                return std::string();
        }

        if (p.is_absolute() && std::experimental::filesystem::exists(p)) {
                return path;
        }

        if (p.is_relative() && std::experimental::filesystem::exists(p)) {
                const std::experimental::filesystem::path abs_path = std::experimental::filesystem::absolute(p);
                return abs_path.string();
        }

        for (auto &&dir : search_directories) {
                if (dir.contains(p)) {
                        const std::experimental::filesystem::path s { std::experimental::filesystem::absolute(path) };
                        return s.string();
                }
        }

        return std::string();
}
