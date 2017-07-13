#include "Configuration.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <boost/filesystem.hpp>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

#include "Console.hpp"

struct Directory
{
        // TODO: Probably move this somewhere else ... Maybe the Lexer?
        Directory(const std::string &_directory, const std::string _name = "")
                : directory(_directory), name(_name), exists(boost::filesystem::exists(directory))
                  , is_directory(boost::filesystem::is_directory(directory))
        { }

        bool contains(const boost::filesystem::path &path)
        {
                (void) path;

                if (!(exists && is_directory)) {
                        if (Config::is_set(Config::VERBOSE)) {
                                std::stringstream ss;
                                ss << "Ignoring invalid directory \"" << name << '"';
                                Console::write_line(ss.str());
                        }
                        return false;
                }

                if (Config::is_set(Config::VERBOSE)) {
                        std::stringstream ss;
                        ss << "Searching \"" << name << '"';
                        Console::write_line(ss.str());
                }

                return false;
        }

        boost::filesystem::path directory;
        std::string             name;

        bool exists;
        bool is_directory;
};

static std::vector<Directory> search_directories;

std::size_t Config::configuration = 0;

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
        boost::filesystem::path p { path };

        if (p.is_absolute() && boost::filesystem::exists(p)) {
                return path;
        }

        for (auto &&dir : search_directories) {
                if (dir.contains(p)) {
                        boost::filesystem::path s { boost::filesystem::absolute(path) };
                        return s.string();
                }
        }

        return std::string();
}
