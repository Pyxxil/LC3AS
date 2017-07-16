#ifndef STRING_MATCHER_HPP
#define STRING_MATCHER_HPP

#include <vector>
#include <string>

class String_Matcher
{
public:
        String_Matcher();
        explicit String_Matcher(const std::string &);

        std::string best_match() const;
        void consider(const std::string &str);

private:
        std::int32_t levenshtein_distance(const std::string &string, const std::string &target) const;

        std::string m_string;

        std::pair<long, std::string> best;
};

#endif // STRING_MATCHER_HPP
