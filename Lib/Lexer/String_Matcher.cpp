#include "Lib/Includes/String_Matcher.hpp"

#include <algorithm>
#include <cstdlib>
#include <limits>

String_Matcher::String_Matcher()
    : m_string(), best{std::numeric_limits<long>::max(), ""}
{}

String_Matcher::String_Matcher(const std::string &t_string)
    : m_string(t_string), best{std::numeric_limits<long>::max(), ""}
{}

std::string String_Matcher::best_match() const
{
    return best.second;
}

void String_Matcher::consider(const std::string &str)
{
    auto length_difference = std::abs(static_cast<int>(str.length() - m_string.length()));
    if (length_difference > best.first) {
        return;
    }

    auto cutoff = static_cast<int>(std::max(m_string.length(), str.length())) / 2;
    if (length_difference > cutoff) {
        return;
    }

    auto distance = static_cast<int>(levenshtein_distance(m_string, str));
    if (distance <= best.first || (str.length() > m_string.length() && best.second.length() < m_string.length())) {
        /* The second half of this if statement should help in cases where we have the following:
         * Valid labels = { oct, b }
         * m_string = oc
         * Without the second half of the if statement, this will get a best match of 'b', but it's likely
         * that oct is a better guess.
         */
        best = {distance, str};
    }
}

std::int32_t String_Matcher::levenshtein_distance(const std::string &string, const std::string &target) const
{
    const size_t string_length = string.length();
    const size_t target_length = target.length();

    if (string_length == 0) {
        return static_cast<std::int32_t>(target_length);
    }

    if (target_length == 0) {
        return static_cast<std::int32_t>(string_length);
    }

    std::vector<size_t> matrix0(string_length + 1);
    std::vector<size_t> matrix1(string_length + 1);

    for (size_t i = 0; i < string_length + 1; ++i) {
        matrix0[0] = i;
    }

    for (size_t i = 0; i < target_length; ++i) {
        matrix1[0] = i + 1;

        for (size_t j = 0; j < string_length; j++) {
            const size_t cost = (string[j] == target[i] ? 0 : 1);
            const size_t deletion = matrix1[j] + 1;
            const size_t insertion = matrix0[j + 1] + 1;
            const size_t substitution = matrix0[j] + cost;
            size_t cheapest = std::min(deletion, insertion);
            cheapest = std::min(cheapest, substitution);
            matrix1[j + 1] = cheapest;
        }

        for (size_t j = 0; j < string_length + 1; j++) {
            matrix0[j] = matrix1[j];
        }
    }

    return static_cast<std::int32_t>(matrix1[string_length]);
}
