#ifndef LC3_ASSEMBLER_DIAGNOSTICS_HPP
#define LC3_ASSEMBLER_DIAGNOSTICS_HPP

#include <string>
#include <stack>
#include <queue>

#include "Console.hpp"

namespace Diagnostics
{
        enum DIAGNOSTIC
        {
                WARNING = 0,
                ERROR   = 1,
                NOTE    = 2,
        };

        enum DIAGNOSTIC_TYPE
        {
                SYNTAX,
                SPELLING,
        };

        class Diagnostic_Context
        {
        public:
                Diagnostic_Context()
                        : selector('^'), highlighter('~'), next(nullptr), show_column(true), show_selector(true)
                          , show_highlights(false), column(0), line_number(0), highlight_length(0), highlight_direction(NONE)
                          , context(), file(), message()
                { }
                Diagnostic_Context(const Diagnostic_Context &other)
                        : selector(other.selector), highlighter(other.highlighter), next(other.next)
                          , show_column(other.show_column), show_selector(other.show_selector)
                          , show_highlights(other.show_highlights), column(other.column)
                          , line_number(other.line_number), highlight_length(other.highlight_length)
                          , highlight_direction(other.highlight_direction), context(other.context)
                          , file(other.file), message(other.message)
                { }
                Diagnostic_Context &operator =(const Diagnostic_Context &&other)
                {
                        if (this != &other) {
                                selector            = other.selector;
                                highlighter         = other.highlighter;
                                next                = other.next;
                                show_column         = other.show_column;
                                show_selector       = other.show_selector;
                                show_highlights     = other.show_highlights;
                                column              = other.column;
                                line_number         = other.line_number;
                                highlight_length    = other.highlight_length;
                                highlight_direction = other.highlight_direction;
                                context             = other.context;
                                file                = other.file;
                                message             = other.message;
                        }
                        return *this;
                }

                Diagnostic_Context &operator =(const Diagnostic_Context &other)
                {
                        selector = other.selector;
                        highlighter = other.highlighter;
                        next = other.next;
                        show_column = other.show_column;
                        show_selector = other.show_selector;
                        show_highlights = other.show_highlights;
                        column = other.column;
                        line_number = other.line_number;
                        highlight_length = other.highlight_length;
                        highlight_direction = other.highlight_direction;
                        context = other.context;
                        file = other.file;
                        message = other.message;
                        return *this;
                }

                static void operator delete(void *ptr, std::size_t size)
                {
                        if (static_cast<Diagnostic_Context *>(ptr)->next) {
                                delete static_cast<Diagnostic_Context *>(ptr)->next;
                        }

                        ::operator delete(ptr, size);
                }

                ~Diagnostic_Context()
                {
                        if (next) {
                                delete next;
                        }
                }

                enum CONTEXT_DIRECTION
                {
                        NONE,
                        RIGHT,
                        LEFT,
                };

                friend std::ostream &operator <<(std::ostream &os, const Diagnostic_Context &con)
                {
                        os << "In " << con.file << ':' << con.line_number << ':';
                        if (con.show_column) {
                                os << con.column << ':';
                        }
                        os << con.message << '\n';

                        return os << con.context;
                }

        private:
                char selector = '^';
                char highlighter = '~';

                Diagnostic_Context *next = nullptr;

                bool show_column;
                bool show_selector;
                bool show_highlights;

                std::size_t column;
                std::size_t line_number;
                std::size_t highlight_length;

                CONTEXT_DIRECTION highlight_direction;

                std::string context;
                std::string file;
                std::string message;
        };

        class Diagnostic_Message
        {
        public:
                Diagnostic_Message()
                        : message(), d_type(), d()
                { }
                explicit Diagnostic_Message(std::string &&t_message, DIAGNOSTIC_TYPE t_type, DIAGNOSTIC diagnostic)
                        : message(t_message), d_type(t_type), d(diagnostic), _context(false)
                { }
                Diagnostic_Message(const Diagnostic_Message &other)
                        : message(other.message), d_type(other.d_type), d(other.d), context(other.context)
                          , _context(other._context)
                {

                }
                Diagnostic_Message &operator =(const Diagnostic_Message &&other)
                {
                        message = other.message;
                        d       = other.d;
                        d_type  = other.d_type;
                        return *this;
                }
                Diagnostic_Message &operator =(const Diagnostic_Message &other)
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

                void provide_context(const Diagnostic_Context &&t_context)
                {
                        context = t_context;
                        _context = true;
                }

                const Diagnostic_Context &given_context()
                {
                        return context;
                }

                const std::string &to_string() const
                {
                        return message;
                }

                DIAGNOSTIC_TYPE type() const
                {
                        return d_type;
                }

                bool has_context() const
                {
                        return _context;
                }

        private:
                // TODO: Add some sort of contextual string.
                std::string     message;
                DIAGNOSTIC_TYPE d_type;
                DIAGNOSTIC      d;

                Diagnostic_Context context;
                bool _context;
        };

        void push(Diagnostic_Message &&message);
        void push(Diagnostic_Message &message);

        std::size_t count();

        void unwind();
};

#endif
