#ifndef LC3_ASSEMBLER_DIAGNOSTICS_HPP
#define LC3_ASSEMBLER_DIAGNOSTICS_HPP

#include <string>
#include <memory>
#include <queue>

#include "Context.hpp"

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

        struct diagnostic_type
        {
                diagnostic_type(const std::string &t, const Console::Colour &col)
                        : type(t), colour(col)
                { }

                friend std::ostream &operator <<(std::ostream &os, const diagnostic_type &t)
                {
                        return os << t.colour << t.type << Console::reset;
                }

                std::string     type;
                Console::Colour colour;
        };

        extern const diagnostic_type diagnostic_colours[];

        class Diagnostic
        {
        public:
                Diagnostic(const FileContext &file);
                Diagnostic(const FileContext &file,
                           std::string &&t_message,
                           DIAGNOSTIC_TYPE t_type,
                           DIAGNOSTIC diagnostic);
                Diagnostic(Diagnostic &other);

                Diagnostic &operator =(Diagnostic &rhs);
                Diagnostic &operator =(Diagnostic &&rhs);

                Diagnostic &operator ()();

                inline void provide_context(std::unique_ptr<Context> &&t_context)
                {
                        context.emplace_back(std::move(t_context));
                }

                inline DIAGNOSTIC type() const
                {
                        return d;
                }

                inline bool has_context() const
                {
                        return context.size() != 0;
                }

        private:
                friend std::ostream &operator <<(std::ostream &os, const Diagnostic &d_msg);

                std::string     message;
                DIAGNOSTIC_TYPE d_type;
                DIAGNOSTIC      d;

                std::vector<std::unique_ptr<Context>> context;

                FileContext info;
        };

        void push(Diagnostic &&message);
        void push(Diagnostic &message);

        std::size_t count();

        void unwind();
}

#endif
