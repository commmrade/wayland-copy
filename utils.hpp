#include <string>
#include <algorithm>
#include <ctype.h>

inline void trim(std::string &s) {
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), isspace));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), isspace).base(), s.end());
}
