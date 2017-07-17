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
    explicit Variant(T var, Console::Colour col = Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
        : information(var), colour(std::move(col))
    {}
    Variant(const Variant<T> &other) = default;
    Variant(Variant<T> &&other) noexcept
        : information(other.information), colour(std::move(other.colour))
    {}
    Variant &operator=(const Variant<T> &rhs) = default;
    Variant &operator=(Variant<T> &&rhs) = default;

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
    T information;
    Console::Colour colour;
};

class FileContext
{
public:
    FileContext(std::string name, size_t t_line, size_t t_col);
    FileContext(Variant<std::string> name, Variant<size_t> t_line, Variant<size_t> t_col);
    FileContext(const FileContext &other) = default;
    FileContext(FileContext &&other) = default;
    FileContext &operator=(const FileContext &rhs) = default;
    FileContext &operator=(FileContext &&rhs) = default;

    ~FileContext() = default;

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
    Variant<size_t> column;
    Variant<size_t> line;
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

    Context(FileContext file, std::string t_message, std::string t_line, CONTEXT_TYPE t_type = NONE);
    Context(const Context &other) = default;
    Context(Context &&other) = default;

    Context &operator=(const Context &rhs) = default;
    Context &operator=(Context &&rhs) = default;

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

class SelectionContext: public Context
{
public:
    SelectionContext(FileContext file, char t_selector, std::string t_message,
                     std::string t_line, std::string changer = std::string());
    SelectionContext(const SelectionContext &other) = default;
    SelectionContext(SelectionContext &&other) = default;

    SelectionContext &operator=(const SelectionContext &rhs) = default;
    SelectionContext &operator=(SelectionContext &&rhs) = default;

    ~SelectionContext() override = default;

    std::ostream &write_to(std::ostream &os) const override;

private:
    char selector;

    std::string fix_it;
};

class HighlightContext: public Context
{
public:
    HighlightContext(SelectionContext t_selector, char t_highlighter,
                     int t_highlight_length, std::string changer = std::string());

    std::ostream &write_to(std::ostream &os) const override;

private:
    char highlighter;

    int highlight_length;

    SelectionContext selector;

    std::string fix_it;
};
}

#endif
