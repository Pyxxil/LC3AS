#ifndef STRING_MATCHER_HPP
#define STRING_MATCHER_HPP

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

class String_Matcher {
public:
  String_Matcher()
      : m_string{}, best{std::numeric_limits<long>::max(), std::string()} {}
  explicit String_Matcher(const std::string &t_string)
      : m_string(t_string), best{std::numeric_limits<long>::max(),
                                 std::string()} {}

  const std::string &best_match() const { return best.second; }

  void consider(const std::string &str) {
    const long length_difference{
        std::abs(static_cast<long>(str.length() - m_string.length()))};
    if (length_difference > best.first) {
      return;
    }

    const long cutoff{
        static_cast<long>(std::max(m_string.length(), str.length())) / 2};
    if (length_difference > cutoff) {
      return;
    }

    const long distance{levenshtein_distance(m_string, str)};
    auto &&[dist, string] = best;
    if (distance <= dist || (str.length() > m_string.length() &&
                             string.length() < m_string.length())) {
      /* The second half of this if statement should help in cases where we
       * have the following: Valid labels = { oct, b } m_string = oc Without
       * the second half of the if statement, this will get a best match of
       * 'b', but it's likely that oct is a better guess.
       */
      best = {distance, str};
    }
  }

private:
  long levenshtein_distance(const std::string &string,
                            const std::string &target) const {
    const size_t string_length{string.length()};
    const size_t target_length{target.length()};

    if (0 == string_length) {
      return static_cast<long>(target_length);
    }

    if (0 == target_length) {
      return static_cast<long>(string_length);
    }

    std::vector<size_t> matrix0(string_length + 1);
    std::vector<size_t> matrix1(string_length + 1);

    for (size_t i = 0; i < string_length + 1; ++i) {
      matrix0[0] = i;
    }

    for (size_t i = 0; i < target_length; ++i) {
      matrix1[0] = i + 1;

      for (size_t j = 0; j < string_length; j++) {
        matrix1[j + 1] =
            std::min(std::min(matrix1[j] + 1, matrix0[j + 1] + 1),
                     matrix0[j] + static_cast<size_t>(string[j] == target[i]));
      }

      for (size_t j = 0; j < string_length + 1; j++) {
        matrix0[j] = matrix1[j];
      }
    }

    return static_cast<long>(matrix1[string_length]);
  }

  std::string m_string;

  std::pair<long, std::string> best;
};

#endif // STRING_MATCHER_HPP
