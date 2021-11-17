#include <string>

namespace utf8 {

    bool is_utf8(const char* str);

    bool is_utf8(const std::string& str);

    std::string fix_utf8(const std::string& src, const std::string& replacement);

    /**
     * @brief Calculates length of an UTF-8 string in characters.
     * 
     * @param src 
     * @param replacement 
     * @return std::string 
     */
    size_t length(const std::string& str);
    size_t length(const char* str);

    std::string to_lower(const std::string &str);
    std::string to_upper(const std::string &str);
}