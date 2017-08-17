#ifndef LINE_HPP
#define LINE_HPP

#include <string>

class Line
{
public:
  explicit Line(const std::string& t_line)
    : m_line(t_line)
  {}

  Line(const Line& other)
    : m_line(other.m_line)
    , m_index(other.m_index)
    , m_ignores(other.m_ignores)
  {}

  Line(Line&& other)
    : m_line(other.m_line)
    , m_index(other.m_index)
    , m_ignores(other.m_ignores)
  {}

  Line& operator=(const Line& other)
  {
    m_line = other.m_line;
    m_index = other.m_index;
    m_ignores = other.m_ignores;
    return *this;
  }

  Line& operator=(Line&& other)
  {
    m_line = other.m_line;
    m_index = other.m_index;
    m_ignores = other.m_ignores;
    return *this;
  }

  ~Line() = default;

  enum IGNORES
  {
    RESET = 0,
    ESCAPE_SEQUENCE = 1,
  };

  /*! Ignore specifc things about the file while finding something
   *
   * @param to_ignore The things to ignore.
   */
  void ignore(size_t to_ignore)
  {
    if (RESET == to_ignore) {
      m_ignores = 0;
    } else {
      m_ignores |= to_ignore;
    }
  }

  char peek() const
  {
    if (at_end()) {
      return 0;
    }

    return m_line.at(m_index);
  }

  /*! Retrieve the next character from the string
   *
   * @return The character read if we're not at the end of the string, 0
   * otherwise.
   */
  char next()
  {
    if (at_end()) {
      return 0;
    }

    const char c = peek();
    ++m_index;
    return c;
  }

  void skip() { ++m_index; }

  /*! Keep advancing until pred becomes false.
   *
   * @param pred The function to call to check if we should stop.
   */
  void skip_if(std::function<bool(const char)>&& pred)
  {
    while (!at_end()) {
      if (!pred(peek())) {
        return;
      }
      ++m_index;
    }
  }

  /*! Attempt to find a character in the string
   *
   * @param needle The character to find
   *
   * @return The index of the character, or -1 if it wasn't found
   */
  size_t find_next(const char needle)
  {
    while (!at_end()) {
      if (needle == peek()) {
        // TODO: Make this a little nicer somehow.
        if (0u == (m_ignores & ESCAPE_SEQUENCE)) {
          return ++m_index;
        } else if ('\\' != m_line.at(m_index - 1)) {
          return ++m_index;
        }
      }
      ++m_index;
    }

    // Couldn't find it
    return -1u;
  }

  size_t find_if(std::function<bool(char)>&& pred)
  {
    while (!at_end()) {
      if (pred(peek())) {
        return m_index;
      }
      next();
    }

    // Didn't work
    return -1u;
  }

  /*! Grab a sub string from within the string
   *
   */
  std::string substr(size_t begin, size_t end) const
  {
    return m_line.substr(begin, end - begin);
  }

  /*! Check if we've reached the end of the string
   *
   * @return true if we have reached the end of the string, otherwise false.
   */
  bool at_end() const { return m_index >= m_line.length(); }

  const Line& operator>>(char& c)
  {
    c = next();
    return *this;
  }

  size_t index() const { return m_index; }

private:
  std::string m_line;
  size_t m_index = 0;
  size_t m_ignores = 0;
};

#endif