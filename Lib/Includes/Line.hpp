#ifndef LINE_HPP
#define LINE_HPP

#include <array>
#include <functional>
#include <string>

class Line {
public:
  explicit Line(const std::string &t_line) : m_line(t_line) {}

  enum IGNORES {
    RESET = 0,
    ESCAPE_SEQUENCE = 1,
  };

  /*! Ignore specific things about the file while finding something
   *
   * @param to_ignore The things to ignore.
   */
  inline void ignore(size_t to_ignore) {
    m_ignores = RESET == to_ignore ? 0 : m_ignores | to_ignore;
  }

  /*! Take a look at the next character in the line.
   *
   * @return The next character in the line, unless we're at the end of the
   * line, in which case 0.
   */
  inline char peek() const { return at(m_index); }

  /*! Retrieve the next character from the string
   *
   * @return The character read if we're not at the end of the string, 0
   * otherwise.
   */
  char next() {
    const auto c = peek();
    if (0 != c) {
      ++m_index;
    }
    return c;
  }

  /*! Skip the next character
   *
   */
  inline void skip() { ++m_index; }

  /*! Keep advancing until pred becomes false.
   *
   * @param pred The function to call to check if we should stop.
   */
  template <typename Func> void skip_while(Func &&pred) {
    while (!at_end()) {
      if (!pred(peek())) {
        return;
      }
      skip();
    }
  }

  /*! Attempt to find a character in the string
   *
   * @param needle The character to find
   *
   * @return The index of the character, or -1 if it wasn't found
   */
  size_t find_next(const char needle) {
    while (!at_end()) {
      if (needle == peek()) {
        // TODO: Make this a little nicer somehow.
        if (0u == (m_ignores & ESCAPE_SEQUENCE)) {
          return ++m_index;
        }

        if (ignores[m_ignores](needle)) {
          return ++m_index;
        }
      }
      skip();
    }

    // Couldn't find it
    return static_cast<size_t>(-1u);
  }

  /*! Keep searching until pred becomes true
   *
   * @param pred The function to call to see if the current character matches
   * some condition
   *
   * @returns The index of the character if pred is ever true, -1 otherwise.
   */
  template <typename Func> size_t find_if(Func &&pred) {
    while (!at_end()) {
      if (pred(peek())) {
        return m_index;
      }
      skip();
    }

    // Didn't find it
    return static_cast<size_t>(-1u);
  }

  /*! Grab a sub string from within the string
   *
   * @param begin The beginning column of the string
   * @param end The end column for the string
   *
   * @return The substring in the line.
   */
  std::string substr(size_t begin, size_t end) const {
    return m_line.substr(begin, end - begin);
  }

  /*! Check if we've reached the end of the string
   *
   * @return true if we have reached the end of the string, otherwise false.
   */
  inline bool at_end() const { return m_index >= m_line.length(); }

  /*! Return the character at a specified index (doesn't do bounds checking)
   *
   * @param index The index of the character
   *
   * @return The character at the index
   */
  inline char at(size_t index) const {
    return static_cast<char>((index >= m_line.length()) ? 0 : (*this)[index]);
  }

  inline char operator[](size_t index) const { return m_line[index]; }

  const Line &operator>>(char &c) {
    c = next();
    return *this;
  }

  inline size_t index() const { return m_index; }

  const std::string &line() const { return m_line; }

private:
  std::string m_line;
  size_t m_index = 0;
  size_t m_ignores = 0;

  const std::array<std::function<bool(char)>, 2> ignores{
      [](char) -> bool {
        return true;
      }, // Not strictly needed, but it doesn't hurt.
      [this](char) -> bool { return (*this)[index() - 1] != '\\'; }};
};

#endif
