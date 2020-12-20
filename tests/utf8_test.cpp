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

TEST(IsUTF8Test, empty)
{
    EXPECT_TRUE(is_utf8(""));
}

TEST(IsUTF8Test, sequence_1_byte)
{
    char buf[] = "1234";
    buf[1] = 0xFF;

    EXPECT_TRUE(is_utf8("abcd"));
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_first)
{
    char buf[] = "фЫваолдж";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0xFF;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_second)
{
    char buf[] = "фЫваолдж";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_2_bytes_C2)
{
    char buf[] = "фЫваолдж";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0xC1;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_first)
{
    char buf[] = "_ह_€_한_";

    EXPECT_TRUE(is_utf8(buf));
    buf[1] = 0xFF;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_second)
{
    char buf[] = "_ह_€_한_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_third)
{
    char buf[] = "_ह_€_한_";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_E0)
{
    char buf[] = "_ह_€_한_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0x9F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_3_bytes_ED)
{
    char buf[] = "_한_€_ह_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0xA1;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_first)
{
    char buf[] = "_𐍈_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[1] = 0xFF;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_second)
{
    char buf[] = "_𐍈_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_third)
{
    char buf[] = "_𐍈_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_fourth)
{
    char buf[] = "_𐍈_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[4] = 0x7F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_F0)
{
    char buf[] = "_𐍈_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0x8F;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_4_bytes_F4)
{
    char buf[] = "_􏿿_𐍈_😁_";

    EXPECT_TRUE(is_utf8(buf));
    buf[2] = 0x90;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_2_bytes)
{
    char buf[] = "1234";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0xC2;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_3_bytes)
{
    char buf[] = "1234";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0xE1;
    EXPECT_FALSE(is_utf8(buf));
    buf[2] = 0xE1;
    buf[3] = 0x80;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(IsUTF8Test, sequence_trim_4_bytes)
{
    char buf[] = "1234";

    EXPECT_TRUE(is_utf8(buf));
    buf[3] = 0xF1;
    EXPECT_FALSE(is_utf8(buf));
    buf[2] = 0xF1;
    buf[3] = 0x80;
    EXPECT_FALSE(is_utf8(buf));
    buf[1] = 0xF1;
    buf[2] = 0x80;
    EXPECT_FALSE(is_utf8(buf));
}

TEST(FixUTF8Test, empty)
{
    EXPECT_EQ(fix_utf8("", "*"), "");
}

TEST(FixUTF8Test, sequence_1_byte)
{
    char buf[] = "1234";
    buf[1] = 0xFF;

    EXPECT_EQ(fix_utf8(buf, "*"), "1*34");
    buf[3] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "1*3*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**3*");
}

TEST(FixUTF8Test, sequence_2_bytes_first)
{
    char buf[] = "фЫваолдж";
    buf[2] = 0xFF;

    EXPECT_EQ(fix_utf8(buf, "*"), "ф**ваолдж");
    buf[14] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "ф**ваолд**");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "****ваолд**");
}

TEST(FixUTF8Test, sequence_2_bytes_second)
{
    char buf[] = "фЫваолдж";
    buf[3] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "ф*ваолдж");
    buf[15] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "ф*ваолд*");
    buf[1] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "**ваолд*");
}

TEST(FixUTF8Test, sequence_3_bytes_first)
{
    char buf[] = "_ह_€_한";
    buf[1] = 0xFF;

    EXPECT_EQ(fix_utf8(buf, "*"), "_***_€_한");
    buf[9] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "_***_€_***");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "****_€_***");
}

TEST(FixUTF8Test, sequence_3_bytes_second)
{
    char buf[] = "_ह_€_한";
    buf[2] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "_*_€_한");
    buf[10] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "_*_€_*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_€_*");
}

TEST(FixUTF8Test, sequence_3_bytes_third)
{
    char buf[] = "_ह_€_한";
    buf[3] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "_*_€_한");
    buf[11] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "_*_€_*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_€_*");
}

TEST(FixUTF8Test, sequence_4_bytes_first)
{
    char buf[] = "_􏿿_𐍈_😁";
    buf[1] = 0xFF;

    EXPECT_EQ(fix_utf8(buf, "*"), "_****_𐍈_😁");
    buf[11] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "_****_𐍈_****");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "*****_𐍈_****");
}

TEST(FixUTF8Test, sequence_4_bytes_second)
{
    char buf[] = "_􏿿_𐍈_😁";
    buf[2] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_😁");
    buf[12] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_𐍈_*");
}

TEST(FixUTF8Test, sequence_4_bytes_third)
{
    char buf[] = "_􏿿_𐍈_😁";
    buf[3] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_😁");
    buf[13] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_𐍈_*");
}

TEST(FixUTF8Test, sequence_4_bytes_fourth)
{
    char buf[] = "_􏿿_𐍈_😁";
    buf[4] = 0x7F;

    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_😁");
    buf[14] = 0x7F;
    EXPECT_EQ(fix_utf8(buf, "*"), "_*_𐍈_*");
    buf[0] = 0xFF;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_𐍈_*");
}

TEST(FixUTF8Test, sequence_2_bytes_1_char_trim)
{
    char buf1[] = "1";
    buf1[0] = 0xC2;

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
}

TEST(FixUTF8Test, sequence_3_bytes_1_char_trim)
{
    char buf1[] = "1";
    buf1[0] = 0xE0;
    char buf2[] = "12";
    buf2[0] = 0xE0;
    buf2[1] = 0xA0;

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
    EXPECT_EQ(fix_utf8(buf2, "*"), "*");
}

TEST(FixUTF8Test, sequence_4_bytes_1_char_trim)
{
    char buf1[] = "1";
    buf1[0] = 0xF0;
    char buf2[] = "12";
    buf2[0] = 0xF4;
    buf2[1] = 0x80;
    char buf3[] = "123";
    buf3[0] = 0xF4;
    buf3[1] = 0x80;
    buf3[2] = 0x80;

    EXPECT_EQ(fix_utf8(buf1, "*"), "*");
    EXPECT_EQ(fix_utf8(buf2, "*"), "*");
    EXPECT_EQ(fix_utf8(buf3, "*"), "*");
}

TEST(FixUTF8Test, sequence_imcomplete)
{
    char buf[] = "01Ы4_€9_😁";
    buf[0] = 0xC2;
    buf[6] = 0xF0;
    buf[10] = 0xE0;

    EXPECT_EQ(fix_utf8(buf, "*"), "*Ы4_****");
    buf[2] = 0xE1;
    EXPECT_EQ(fix_utf8(buf, "*"), "**_****");
}