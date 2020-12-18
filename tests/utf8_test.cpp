#include "gtest/gtest.h"
#include <utf8.h>

using namespace utf8;

TEST(UTF8Test, IsUTF8)
{
    char buf[] = "1234";
    bool res1 = is_utf8("abcd");
    buf[1] = 0xFF;
    bool res2 = is_utf8(buf);

    EXPECT_TRUE(res1);
    EXPECT_FALSE(res2);
}