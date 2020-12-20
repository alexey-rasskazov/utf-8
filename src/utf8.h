#include <string>

namespace utf8 {

    bool is_utf8(const char* str);

    bool is_utf8(const std::string& str);

    std::string fix_utf8(const std::string& src, const std::string& replacement);
}