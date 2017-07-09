#ifndef LC3_ASSEMBLER_DIAGNOSTICS_HPP
#define LC3_ASSEMBLER_DIAGNOSTICS_HPP

#include <stack>
#include <queue>
#include <string>

namespace Diagnostics
{
        enum DIAGNOSTIC
        {
                WARNING,
                ERROR,
        };

        enum DIAGNOSTIC_TYPE
        {
                SYNTAX,
                SPELLING,
        };

        class Diagnostic_Message
        {
        public:
                Diagnostic_Message()
                        : message(), d_type(), d()
                { }
                explicit Diagnostic_Message(std::string &&t_message, DIAGNOSTIC_TYPE t_type, DIAGNOSTIC diagnostic)
                        : message(t_message), d_type(t_type), d(diagnostic)
                { }
                Diagnostic_Message(const Diagnostic_Message &other)
                        : message(other.message), d_type(other.d_type), d(other.d)
                {

                }
                Diagnostic_Message &operator =(const Diagnostic_Message &&other)
                {
                        message = other.message;
                        d       = other.d;
                        d_type  = other.d_type;
                        return *this;
                }

                Diagnostic_Message &operator <<(const std::string &&extra)
                {
                        message += extra;
                        return *this;
                }

                Diagnostic_Message &operator ()()
                {
                        return *this;
                }

                std::string &to_string()
                {
                        return message;
                }

        private:
                // TODO: Add some sort of contextual string.
                std::string     message;
                DIAGNOSTIC_TYPE d_type;
                DIAGNOSTIC      d;
        };

        void push(Diagnostic_Message &&message);
        void push(Diagnostic_Message &message);

        static Diagnostic_Message &pop();

        static std::queue<Diagnostic_Message> diagnostics_log;

        void unwind();
};

#endif
