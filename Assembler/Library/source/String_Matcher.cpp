#include <cstdlib>
#include "String_Matcher.hpp"

String_Matcher::String_Matcher()
{

}

String_Matcher::String_Matcher(const std::string &t_string)
        : m_string(t_string), best{LONG_MAX, "" }
{

}

std::string String_Matcher::best_match() const
{
        return best.second;
}

void String_Matcher::consider(const std::string &str)
{
        const int length_difference = std::abs(static_cast<int>(str.length() - m_string.length()));
        if (length_difference >= best.first) {
                return;
        }

        const int cutoff = static_cast<int>(std::max(m_string.length(), str.length())) / 2;
        if (length_difference > cutoff) {
                return;
        }

        const int distance = static_cast<int>(levenshtein_distance(m_string, str));
        if (distance < best.first) {
                best = {distance, str};
        }
}

std::int32_t String_Matcher::levenshtein_distance(const std::string &string, const std::string &target) const
{
        const std::size_t string_length = string.length();
        const std::size_t target_length = target.length();

        if (string_length == 0) {
                return static_cast<std::int32_t>(target_length);
        } else if (target_length == 0) {
                return static_cast<std::int32_t>(string_length);
        }

        std::vector<std::size_t> matrix0(string_length + 1);
        std::vector<std::size_t> matrix1(string_length + 1);

        for (std::size_t i = 0; i < string_length + 1; ++i) {
                matrix0[0] = i;
        }

        for (std::size_t i = 0; i < target_length; ++i) {
                matrix1[0] = i + 1;

                for (std::size_t j = 0; j < string_length; j++) {
                        const std::size_t cost = (string[j] == target[i] ? 0 : 1);
                        const std::size_t deletion     = matrix1[j] + 1;
                        const std::size_t insertion    = matrix0[j + 1] + 1;
                        const std::size_t substitution = matrix0[j] + cost;
                        std::size_t cheapest = std::min(deletion, insertion);
                        cheapest = std::min(cheapest, substitution);
                        matrix1[j + 1] = cheapest;
                }

                for (std::size_t j = 0; j < string_length + 1; j++) {
                        matrix0[j] = matrix1[j];
                }
        }

        return static_cast<std::int32_t>(matrix1[string_length]);
}
