#include "Logging/Logger.hpp"

#include <iostream>
#include <sstream>

Logger::Logger()
        : Logger(WARNING_TYPE::ALL, false)
{

}

Logger::Logger(int warning_level, bool quietness)
        : m_warn(warning_level), quiet(quietness)
{

}

void Logger::LOG(LOGGING_TYPE level, int line_number, const std::string &&message, WARNING_TYPE warning_level)
{
        if (we_should_be_quiet()) {
                return;
        }

        if (level == ERROR) {
                ERR(line_number, message);
        } else if (level == WARNING) {
                WARN(warning_level, line_number, message);
        } else {
                if (line_number) {
                        std::cout << "Line " << line_number << ": ";
                }
                std::cout << message;
        }
}

void Logger::WARN(WARNING_TYPE level, int line_number, const std::string &warning)
{
        if (!(m_warn & level)) {
                return;
        }

        std::stringstream stream;

        stream << "WARNING: ";

        if (line_number) {
                stream << "Line " << line_number << ": ";
        }

        stream << warning << "\n";

        std::cerr << stream.str();
}

void Logger::ERR(int line_number, const std::string &error)
{
        std::stringstream stream;

        stream << "ERROR: ";

        if (line_number) {
                stream << "Line " << line_number << ": ";
        }

        stream << error << "\n";

        std::cerr << stream.str();
}

void Logger::set_warning_level(int level)
{
        m_warn = level;
}

void Logger::set_quietness(bool be_quiet)
{
        quiet = be_quiet;
}
