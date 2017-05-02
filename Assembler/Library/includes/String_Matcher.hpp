#ifndef PROJECT_STRING_MATCHER_HPP
#define PROJECT_STRING_MATCHER_HPP

#include <vector>
#include <string>

class String_Matcher
{
public:
        String_Matcher();
        String_Matcher(const std::string &);

        std::string best_match() const;
        void consider(const std::string &str);

private:
        std::int32_t levenshtein_distance(const std::string &string, const std::string &target) const;

        std::string m_string;

        std::pair<long, std::string> best;
};

#endif //PROJECT_STRING_MATCHER_HPP
