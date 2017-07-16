#include "Configuration.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <boost/filesystem.hpp>
#include <utility>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

#include "Diagnostics.hpp"

struct Directory
{
        // TODO: Probably move this somewhere else ... Maybe the Lexer?
        explicit Directory(const std::string &_directory, std::string _name = "")
                : directory(_directory), name(std::move(_name)), exists(boost::filesystem::exists(directory))
                  , is_directory(boost::filesystem::is_directory(directory))
        { }

        bool contains(const boost::filesystem::path &path)
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

        boost::filesystem::path directory;
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

        return std::string();
}
