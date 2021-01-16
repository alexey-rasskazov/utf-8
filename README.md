# UTF-8 C++ Utilities
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/alexey-rasskazov/utf-8.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/alexey-rasskazov/utf-8/context:cpp)

A C++ library containing utility functions that help to operate with UTF-8 encoding.

The function list:

```cpp
bool is_utf8(const char* str);
bool is_utf8(const std::string& str);
 ```
 - checks if a string is UTF-8 encoded
```cpp
std::string fix_utf8(const std::string& src, const std::string& replacement)
```
 - replaces invalid UTF-8 characters by specified symbols

Example:

```cpp
#include <utf8.h>

...

if (utf8::is_utf8(text)) {
    // process "text" as UTF-8
    ...
} else {
    std::cout << "text encoding is not UTF-8" << std::endl;
}

```
