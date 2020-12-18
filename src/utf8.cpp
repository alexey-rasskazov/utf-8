#include "utf8.h"

/**
 *  Check if string is UTF-8
 */
bool utf8::is_utf8(const char* str)
{
    const unsigned char * bytes = (const unsigned char *)str;
	int num;

	while (*bytes != 0x00)
	{
		if ((*bytes & 0x80) == 0x00)
		{
			bytes++;
		}
		else
        {
            if ((*bytes & 0xE0) == 0xC0)
            {
                num = 2;
            }
            else if ((*bytes & 0xF0) == 0xE0)
            {
                num = 3;
            }
            else if ((*bytes & 0xF8) == 0xF0)
            {
                num = 4;
            }
            else
            {
                return false;
            }

            bytes++;

            for (int i = 1; i < num; ++i)
            {
                if ((*bytes & 0xC0) != 0x80)
                {
                    return false;
                }
                bytes++;
            }
        }
	}

    return true;
}

/**
 *  Check if string is UTF-8
 */
bool utf8::is_utf8(const std::string& str)
{
    return is_utf8(str.c_str());
}