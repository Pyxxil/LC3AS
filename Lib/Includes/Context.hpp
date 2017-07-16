#ifndef LC3_ASSEMBLER_CONTEXT_HPP

#define LC3_ASSEMBLER_CONTEXT_HPP

#include <utility>

#include "Console.hpp"

namespace Diagnostics
{
        template <class T>
        class Variant
        {
        public:
                explicit Variant(T var, Console::Colour col = Console::reset)
                        : information(var), colour(std::move(col))
                { }
                Variant(const Variant<T> &other)
                        : information(other.information), colour(other.colour)
                { }
                Variant(Variant<T> &&other) noexcept
                        : information(other.information), colour(other.colour)
                { }
                Variant &operator =(const Variant<T> &rhs)
                {
                        information = rhs.information;
                        colour      = rhs.colour;
                        return *this;
                }
                Variant &operator =(Variant<T> &&rhs) noexcept
                {
                        if (&rhs != this) {
                                information = rhs.information;
                                colour      = rhs.colour;
                        }
                        return *this;
                }

                ~Variant() = default;

                const T &var() const
                {
                        return information;
                }

                std::ostream &write_to(std::ostream &os) const
                {
                        return os << colour << information << Console::reset;
                }

        private:

                T               information;
                Console::Colour colour;
        };

        class FileContext
        {
        public:
                FileContext(std::string &name, size_t t_line, size_t t_col);
                FileContext(std::string &&name, size_t t_line, size_t t_col);
                FileContext(Variant<std::string> &name, Variant<size_t> &t_line, Variant<size_t> &t_col);
                FileContext(Variant<std::string> &&name, Variant<size_t> &&t_line, Variant<size_t> &&t_col);

                inline const Variant<size_t> &get_column() const
                {
                        return column;
                }

                inline std::ostream &write_to(std::ostream &os) const
                {
                        os << "In ";
                        file_name.write_to(os) << ':';
                        line.write_to(os) << ':';
                        return column.write_to(os) << ':';
                }

        private:
                Variant<std::string> file_name;
                Variant<size_t>      column;
                Variant<size_t>      line;
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
                Context(const Context &other) = default;
                Context(const Context &&other) noexcept;

                Context &operator =(const Context &rhs) = default;
                Context &operator =(Context &&rhs) noexcept = default;

                virtual ~Context() = default;

                virtual std::ostream &write_to(std::ostream &os) const;

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
                SelectionContext(const Diagnostics::FileContext &file, char t_selector,
                                 const std::string &t_message, const std::string &t_line,
                                 const std::string &changer);
                SelectionContext(const FileContext &file, char t_selector, const std::string &&t_message,
                                 const std::string &&t_line, const std::string &changer = std::string());
                SelectionContext(const SelectionContext &other)
                        : Context(other.file_information, other.message, other.line, other.context_type)
                          , selector(other.selector), fix_it(other.fix_it)
                { }
                SelectionContext(SelectionContext &&other) noexcept = default;

                SelectionContext &operator =(const SelectionContext &rhs) = default;
                SelectionContext &operator =(SelectionContext &&rhs) noexcept = default;

                ~SelectionContext() override = default;

                std::ostream &write_to(std::ostream &os) const override;

        private:
                char selector;

                std::string fix_it;
        };

        class HighlightContext : public Context
        {
        public:
                HighlightContext(const SelectionContext &t_selector, char t_highlighter,
                                 int t_highlight_length, const std::string &changer = std::string());

                std::ostream &write_to(std::ostream &os) const override;

        private:
                char highlighter;

                int highlight_length;

                SelectionContext selector;

                std::string fix_it;
        };
}

#endif
