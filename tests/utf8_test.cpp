#include "gtest/gtest.h"
#include <utf8.h>

using namespace utf8;

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

using u8t = std::decay_t<decltype(u8""[0])>; // char until C++20, char8_t since C++20

#define U8BUF(text) \
    u8t u8buf[] = text; \
    char *buf = reinterpret_cast<char*>(u8buf);

#define SET_BUF_BYTE(ind, val) \
    ((unsigned char*)buf)[ind] = (unsigned char)(val);

#define SET_BYTE(buf, ind, val) \
    ((unsigned char*)(buf))[ind] = (unsigned char)(val);

TEST(IsUTF8Test, empty)
{
    EXPECT_TRUE(is_utf8(""));
}

TEST(IsUTF8Test, sequence_1_byte)
{
    U8BUF(u8"1234")
    SET_BUF_BYTE(1, 0xFF)

    EXPECT_TRUE(is_utf8("abcd"));
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_first)
{
    U8BUF(u8"\u0444\u042b\u0432\u0430\u043e\u043b\u0434\u0436")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0xFF)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_second)
{
    U8BUF(u8"\u0444\u042b\u0432\u0430\u043e\u043b\u0434\u0436")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_C2)
{
    U8BUF(u8"\u0444\u042b\u0432\u0430\u043e\u043b\u0434\u0436")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0xC1)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_first)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(1, 0xFF)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_second)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_third)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_E0)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0x9F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_ED)
{
    U8BUF(u8"\u005f\ud55c\u005f\u20ac\u005f\u0939\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0xA1)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_first)
{
    U8BUF(u8"\u005f\U00010348\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(1, 0xFF)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_second)
{
    U8BUF(u8"\u005f\U00010348\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_third)
{
    U8BUF(u8"\u005f\U00010348\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_fourth)
{
    U8BUF(u8"\u005f\U00010348\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(4, 0x7F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_F0)
{
    U8BUF(u8"\u005f\U00010348\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0x8F)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_F4)
{
    U8BUF(u8"\u005f\U0010ffff\u005f\U00010348\u005f\U0001f601\u005f")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(2, 0x90)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_2_bytes)
{
    U8BUF(u8"1234")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0xC2)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_3_bytes)
{
    U8BUF(u8"1234")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0xE1)
    EXPECT_FALSE(is_utf8(buf));
    SET_BUF_BYTE(2, 0xE1)
    SET_BUF_BYTE(3, 0x80)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_4_bytes)
{
    U8BUF(u8"1234")

    EXPECT_TRUE(is_utf8(buf));
    SET_BUF_BYTE(3, 0xF1)
    EXPECT_FALSE(is_utf8(buf));
    SET_BUF_BYTE(2, 0xF1)
    SET_BUF_BYTE(3, 0x80)
    EXPECT_FALSE(is_utf8(buf));
    SET_BUF_BYTE(1, 0xF1)
    SET_BUF_BYTE(2, 0x80)
    EXPECT_FALSE(is_utf8(buf));
}

TEST(FixUTF8Test, empty)
{
    EXPECT_EQ(fix_utf8("", "*"), "");
}

TEST(FixUTF8Test, sequence_1_byte)
{
    U8BUF(u8"1234")
    SET_BUF_BYTE(1, 0xFF);

    EXPECT_EQ(fix_utf8(buf, "*"), "1*34");
    SET_BUF_BYTE(3, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), "1*3*");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), "**3*");
}

TEST(FixUTF8Test, sequence_2_bytes_first)
{
    U8BUF(u8"\u0444\u042b\u0432\u0430\u043e\u043b\u0434\u0436")
    SET_BUF_BYTE(2, 0xFF)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u0444\u002a\u002a\u0432\u0430\u043e\u043b\u0434\u0436");
    SET_BUF_BYTE(14, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u0444\u002a\u002a\u0432\u0430\u043e\u043b\u0434\u002a\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u002a\u002a\u0432\u0430\u043e\u043b\u0434\u002a\u002a");
}

TEST(FixUTF8Test, sequence_2_bytes_second)
{
    U8BUF(u8"\u0444\u042b\u0432\u0430\u043e\u043b\u0434\u0436")
    SET_BUF_BYTE(3, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u0444\u002a\u0432\u0430\u043e\u043b\u0434\u0436");
    SET_BUF_BYTE(15, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u0444\u002a\u0432\u0430\u043e\u043b\u0434\u002a");
    SET_BUF_BYTE(1, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u0432\u0430\u043e\u043b\u0434\u002a");
}

TEST(FixUTF8Test, sequence_3_bytes_first)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c")
    SET_BUF_BYTE(1, 0xFF)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u002a\u002a\u005f\u20ac\u005f\ud55c");
    SET_BUF_BYTE(9, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u002a\u002a\u005f\u20ac\u005f\u002a\u002a\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u002a\u002a\u005f\u20ac\u005f\u002a\u002a\u002a");
}

TEST(FixUTF8Test, sequence_3_bytes_second)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c")
    SET_BUF_BYTE(2, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\u20ac\u005f\ud55c");
    SET_BUF_BYTE(10, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\u20ac\u005f\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u005f\u20ac\u005f\u002a");
}

TEST(FixUTF8Test, sequence_3_bytes_third)
{
    U8BUF(u8"\u005f\u0939\u005f\u20ac\u005f\ud55c")
    SET_BUF_BYTE(3, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\u20ac\u005f\ud55c");
    SET_BUF_BYTE(11, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\u20ac\u005f\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u005f\u20ac\u005f\u002a");
}

TEST(FixUTF8Test, sequence_4_bytes_first)
{
    U8BUF(u8"\u005f\U0010ffff\u005f\U00010348\u005f\U0001f601")
    SET_BUF_BYTE(1, 0xFF)

    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u005f\u002a\u002a\u002a\u002a\u005f\U00010348\u005f\U0001f601");
    SET_BUF_BYTE(11, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u005f\u002a\u002a\u002a\u002a\u005f\U00010348\u005f\u002a\u002a\u002a\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u002a\u002a\u002a\u002a\u002a\u005f\U00010348\u005f\u002a\u002a\u002a\u002a");
}

TEST(FixUTF8Test, sequence_4_bytes_second)
{
    U8BUF(u8"\u005f\U0010ffff\u005f\U00010348\u005f\U0001f601")
    SET_BUF_BYTE(2, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\U0001f601");
    SET_BUF_BYTE(12, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"),  (const char*)u8"\u002a\u002a\u005f\U00010348\u005f\u002a");
}

TEST(FixUTF8Test, sequence_4_bytes_third)
{
    U8BUF(u8"\u005f\U0010ffff\u005f\U00010348\u005f\U0001f601")
    SET_BUF_BYTE(3, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\U0001f601");
    SET_BUF_BYTE(13, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u005f\U00010348\u005f\u002a");
}

TEST(FixUTF8Test, sequence_4_bytes_fourth)
{
    U8BUF(u8"\u005f\U0010ffff\u005f\U00010348\u005f\U0001f601")
    SET_BUF_BYTE(4, 0x7F)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\U0001f601");
    SET_BUF_BYTE(14, 0x7F)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u005f\u002a\u005f\U00010348\u005f\u002a");
    SET_BUF_BYTE(0, 0xFF)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u005f\U00010348\u005f\u002a");
}

TEST(FixUTF8Test, sequence_2_bytes_1_char_trim)
{
    char buf1[] = "1";
    SET_BYTE(buf1, 0, 0xC2)

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
}

TEST(FixUTF8Test, sequence_3_bytes_1_char_trim)
{
    char buf1[] = "1";
    SET_BYTE(buf1, 0, 0xE0)
    char buf2[] = "12";
    SET_BYTE(buf2, 0, 0xE0)
    SET_BYTE(buf2, 1, 0xA0)

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
    EXPECT_EQ(fix_utf8(buf2, "*"), "*");
}

TEST(FixUTF8Test, sequence_4_bytes_1_char_trim)
{
    char buf1[] = "1";
    SET_BYTE(buf1, 0, 0xF0)
    char buf2[] = "12";
    SET_BYTE(buf2, 0, 0xF4)
    SET_BYTE(buf2, 1, 0x80)
    char buf3[] = "123";
    SET_BYTE(buf3, 0, 0xF4)
    SET_BYTE(buf3, 1, 0x80)
    SET_BYTE(buf3, 2, 0x80)

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
    EXPECT_EQ(fix_utf8(buf2, "*"), "*");
    EXPECT_EQ(fix_utf8(buf3, "*"), "*");
}

TEST(FixUTF8Test, sequence_imcomplete)
{
    U8BUF(u8"\u0030\u0031\u042b\u0034\u005f\u20ac\u0039\u005f\U0001f601")
    SET_BUF_BYTE(0, 0xC2)
    SET_BUF_BYTE(6, 0xF0)
    SET_BUF_BYTE(10, 0xE0)

    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u042b\u0034\u005f\u002a\u002a\u002a\u002a");
    SET_BUF_BYTE(2, 0xE1)
    EXPECT_EQ(fix_utf8(buf, "*"), (const char*)u8"\u002a\u002a\u005f\u002a\u002a\u002a\u002a");
}