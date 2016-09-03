#include "test.h"
#include "ui/text.h"

void test_symbol(void);

static char *test_hexdump(char *data, size_t n, char *buffer)
{
    char *cur;

    cur = buffer;

    while (n--)
    {
        sprintf(cur, "%02x ", (unsigned char)*(data++));
        cur += 3;
    }

    cur[-1] = '\0';

    return buffer;
}

void test_symbol(void)
{
    char teststr1[] = "ascii a";
    char teststr2[] = "utf8_2wide \xc1\x81";
    char teststr3[] = "utf8_3wide \xe1\x81\x81";
    char teststr4[] = "utf8_4wide \xf1\x81\x81\x81";
    char teststr5[] = "face_start \nCURSA";
    char teststr6[] = "utf8_big   \xfe\x81\x81\x81\x81\x81\x81";

    uint i;
    char buffer[500];

    TEST(test_hexdump(teststr1, sizeof teststr1, buffer), s);
    for (i = 0; i < sizeof teststr1; i++)
        teststr1[i] = (char)ui_text_symbol(teststr1[i]);
    TEST(test_hexdump(teststr1, sizeof teststr1, buffer), s);

    TEST(test_hexdump(teststr2, sizeof teststr2, buffer), s);
    for (i = 0; i < sizeof teststr2; i++)
        teststr2[i] = (char)ui_text_symbol(teststr2[i]);
    TEST(test_hexdump(teststr2, sizeof teststr2, buffer), s);

    TEST(test_hexdump(teststr3, sizeof teststr3, buffer), s);
    for (i = 0; i < sizeof teststr3; i++)
        teststr3[i] = (char)ui_text_symbol(teststr3[i]);
    TEST(test_hexdump(teststr3, sizeof teststr3, buffer), s);

    TEST(test_hexdump(teststr4, sizeof teststr4, buffer), s);
    for (i = 0; i < sizeof teststr4; i++)
        teststr4[i] = (char)ui_text_symbol(teststr4[i]);
    TEST(test_hexdump(teststr4, sizeof teststr4, buffer), s);

    TEST(test_hexdump(teststr5, sizeof teststr5, buffer), s);
    for (i = 0; i < sizeof teststr5; i++)
        teststr5[i] = (char)ui_text_symbol(teststr5[i]);
    TEST(test_hexdump(teststr5, sizeof teststr5, buffer), s);

    TEST(test_hexdump(teststr6, sizeof teststr6, buffer), s);
    for (i = 0; i < sizeof teststr6; i++)
        teststr6[i] = (char)ui_text_symbol(teststr6[i]);
    TEST(test_hexdump(teststr6, sizeof teststr6, buffer), s);

}

int main(void)
{
    RUNTEST(symbol);

    return 0;
}
