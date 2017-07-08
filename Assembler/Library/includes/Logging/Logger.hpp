#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <memory>

namespace Logging
{
        enum LOGGING_TYPE
        {
                MESSAGE = 0,
                ERROR   = 1,
                WARNING = 2,
        };

        enum WARNING_TYPE
        {
                NONE                 = 0,
                SYNTAX               = 1 << 0,
                IGNORED              = 1 << 1,
                MULTIPLE_DEFINITIONS = 1 << 2,
                LOGIC                = 1 << 3,
                ALL                  = SYNTAX | IGNORED | MULTIPLE_DEFINITIONS | LOGIC,
        };

        class Logger
        {
        public:
                Logger();
                Logger(int warning_level, bool quietness);

                void LOG(LOGGING_TYPE level,
                         int line_number,
                         const std::string &file,
                         const std::string &&message,
                         WARNING_TYPE warning_level);

                void set_warning_level(int level);
                void set_quietness(bool be_quiet);

        private:
                int  m_warn = WARNING_TYPE::ALL;
                bool quiet  = false;

                bool we_should_be_quiet() const
                { return quiet; }

                void err(int line_number, const std::string &file, const std::string &error);
                void warn(WARNING_TYPE level, int line_number, const std::string &file, const std::string &warning);
        };

        extern std::unique_ptr<Logger> logger;
}

#endif // LOGGER_HPP
