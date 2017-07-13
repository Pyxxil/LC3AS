#ifndef LC3_ASSEMBLER_CONTEXT_HPP
#define LC3_ASSEMBLER_CONTEXT_HPP

#include "Console.hpp"

namespace Diagnostics
{
        template <class T>
        class Variant
        {
        public:
                Variant(T var, const Console::Colour &col = Console::reset)
                        : information(var), colour(col)
                { }
                Variant(const Variant<T> &other)
                        : information(other.information), colour(other.colour)
                { }
                Variant &operator =(const Variant<T> &rhs)
                {
                        information = rhs.information;
                        colour = rhs.colour;
                        return *this;
                }
                Variant &operator =(const Variant<T> &&rhs)
                {
                        if (&rhs != this) {
                                information = rhs.information;
                                colour = rhs.colour;
                        }
                        return *this;
                }

                const T &var() const
                {
                        return information;
                }
        private:
                friend std::ostream &operator <<(std::ostream &os, const Variant<T> variant)
                {
                        return os << variant.colour << variant.information << Console::reset;
                }

                T information;
                Console::Colour colour;
        };

        class FileContext
        {
        public:
                FileContext(std::string &name, std::size_t t_line, std::size_t t_col);
                FileContext(std::string &&name, std::size_t t_line, std::size_t t_col);
                FileContext(Variant<std::string> &name, Variant<std::size_t> &t_line, Variant<std::size_t> &t_col);
                FileContext(Variant<std::string> &&name, Variant<std::size_t> &&t_line, Variant<std::size_t> &&t_col);

                Variant<std::size_t> column;
        private:
                friend std::ostream &operator <<(std::ostream &os, const FileContext &file);

                Variant<std::string> file_name;
                Variant<std::size_t> line;
        };

        class Context
        {
        public:
                enum CONTEXT_TYPE
                {
                        NONE,
                        SELECTOR,
                        HIGHLIGHT,
                };

                Context(const FileContext &file, const std::string &t_message, const std::string &t_line,
                        CONTEXT_TYPE t_type = NONE);
                Context(const FileContext &file, const std::string &&t_message, const std::string &&t_line,
                        CONTEXT_TYPE t_type = NONE);
                Context(const Context &other);
                Context(const Context &&other);

                Context &operator =(const Context &rhs);
                Context &operator =(const Context &&rhs);

                friend std::ostream &operator <<(std::ostream &os, const Context &con);

                inline CONTEXT_TYPE type() const
                {
                        return context_type;
                }

                FileContext file_information;

                std::string message;
                std::string line;
                std::string empty_line;

                CONTEXT_TYPE context_type;
        };

        class SelectionContext : public Context
        {
        public:
                SelectionContext(const FileContext &file, char t_selector, const std::string &&t_message,
                                 const std::string &&t_line, const std::string &changer = std::string());
                SelectionContext(const SelectionContext &other)
                        : Context(other.file_information, other.message, other.line, other.context_type)
                          , selector(other.selector), change(other.change)
                {}

                friend std::ostream &operator <<(std::ostream &os, const SelectionContext &context);

        private:
                char selector;

                std::string change;
        };

        class HighlightContext : public Context
        {
        public:
                HighlightContext(const SelectionContext &t_selector, char t_highlighter,
                                 int t_highlight_length, const std::string &changer = std::string());

                friend std::ostream &operator <<(std::ostream &os, const HighlightContext &context);

        private:
                char highlighter;

                int highlight_length;

                SelectionContext selector;

                std::string change;
        };
}

#endif
