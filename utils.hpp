#include <cstring>
#include <string>
#include <algorithm>
#include <ctype.h>

namespace Wayland::Utils {


inline void trim(std::string &s) {
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), isspace));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), isspace).base(), s.end());
}

inline bool is_png(const std::string& data) {
    const char pngSignature[8] = { '\x89', 'P', 'N', 'G', '\r', '\n', '\x1a', '\n' };
    return data.size() >= 8 && memcmp(data.data(), pngSignature, 8) == 0;
}

inline bool is_jpeg(const std::string& data) {
    return data.size() >= 2 &&
           static_cast<unsigned char>(data[0]) == 0xFF &&
           static_cast<unsigned char>(data[1]) == 0xD8;
}
}
