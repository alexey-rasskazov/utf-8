#include "utf8.h"

//
// Validation based on conditions from:
//   https://www.unicode.org/versions/Unicode12.0.0/UnicodeStandard-12.0.pdf, page 126
//
// Table 3-7. Well-Formed UTF-8 Byte Sequences
// ┌────────────────────┬────────────┬─────────────┬────────────┬─────────────┐
// │ Code Points        │ First Byte │ Second Byte │ Third Byte │ Fourth Byte │
// ├────────────────────┼────────────┼─────────────┼────────────┼─────────────┤
// │ U+0000..U+007F     │ 00..7F     │             │            │             │
// │ U+0080..U+07FF     │ C2..DF  (1)│ 80..BF      │            │             │
// │ U+0800..U+0FFF     │ E0         │ A0..BF   (2)│ 80..BF     │             │
// │ U+1000..U+CFFF     │ E1..EC     │ 80..BF      │ 80..BF     │             │
// │ U+D000..U+D7FF     │ ED         │ 80..9F   (3)│ 80..BF     │             │
// │ U+E000..U+FFFF     │ EE..EF     │ 80..BF      │ 80..BF     │             │
// │ U+10000..U+3FFFF   │ F0      (4)│ 90..BF   (5)│ 80..BF     │ 80..BF      │
// │ U+40000..U+FFFFF   │ F1..F3  (4)│ 80..BF      │ 80..BF     │ 80..BF      │
// │ U+100000..U+10FFFF │ F4      (4)│ 80..8F   (6)│ 80..BF     │ 80..BF      │
// └────────────────────┴────────────┴─────────────┴────────────┴─────────────┘
//
// As a consequence of the well-formedness conditions specified in Table 3-7, the following
// byte values are disallowed in UTF-8: C0–C1, F5–FF.

const char* find_invalid_byte(const char* str, int& num_bytes)
{
    const unsigned char * bytes = (const unsigned char *)str;
	int num;

	while (*bytes)
	{
        // 1 byte per symbol
		if ((*bytes & 0x80) == 0x00)
		{
			bytes++;
		}
		else
        {
            unsigned char first_byte = *bytes;
            // 2 bytes per symbol
            if ((first_byte & 0xE0) == 0xC0)
            {
                if (first_byte < 0xC2) // check range (1)
                {
                    num_bytes = 2;
                    return (const char*)bytes;
                }
                num = 2;
            }
            // 3 bytes per symbol
            else if ((first_byte & 0xF0) == 0xE0) 
            {
                if (first_byte == 0xE0)
                {
                    if (bytes[1] < 0xA0) // check range (2)
                    {
                        num_bytes = 3;
                        return (const char*)bytes;
                    }
                }
                else if (first_byte == 0xED)
                {
                    if (bytes[1] > 0x9F) // check range (3)
                    {
                        num_bytes = 3;
                        return (const char*)bytes;
                    }
                }
                num = 3;
            }
            else if ((first_byte & 0xF8) == 0xF0)
            {
                if (first_byte > 0xF4)  // check range (4)
                {
                    num_bytes = 4;
                    return (const char*)bytes;
                }

                if (first_byte == 0xF0)
                {
                    if (bytes[1] < 0x90) // check range (5)
                    {
                        num_bytes = 4;
                        return (const char*)bytes;
                    }
                }
                else if (first_byte == 0xF4)
                {
                    if (bytes[1] > 0x8F) // check range (6)
                    {
                        num_bytes = 4;
                        return (const char*)bytes;
                    }
                }
                num = 4;
            }
            else
            {
                num_bytes = 1;
                return (const char*)bytes;
            }

            // check 80..BF (10XXXXXX) trailing bytes
            for (int i = 1; i < num; ++i)
            {
                if ((bytes[i] & 0xC0) != 0x80)
                {
                    num_bytes = num;
                    return (const char*)bytes;
                }
            }

            bytes += num;
        }
	}

    return nullptr;
}

/**
 *  Check if string is UTF-8
 */
bool utf8::is_utf8(const char* str)
{
    int num_bytes;

    return find_invalid_byte(str, num_bytes) == nullptr;
}

/**
 *  Check if string is UTF-8
 */
bool utf8::is_utf8(const std::string& str)
{
    return is_utf8(str.c_str());
}

/**
 * Fix UTF-8 characters
 */
std::string utf8::fix_utf8(const std::string& src, const std::string& replacement)
{
    const char* start = src.c_str();
    int num_bytes;

    const char* pos = find_invalid_byte(start, num_bytes);
    
    if (pos == nullptr)
    {
        return src;
    }
    
    std::string res;
    res.reserve(src.length());
    const char *prev = start;

    do
    {
        if (pos - prev > 0)
        {
            res.append(src.substr(prev - start, pos - prev));
        }
        if (!replacement.empty())
        {
            res.append(replacement);
        }
        prev = pos + num_bytes;
        if (prev - start >= src.length())
        {
            break;
        }
        pos = find_invalid_byte(prev, num_bytes);
    } while (pos);

    if (prev - start < src.length())
    {
        res.append(src.substr(prev - start));
    }

    return res;
}