#include "genconvertstring.h"

#include <hooker.h>
#include <stdint.h>

int32_t GenConvertStringEncoding(
    const void *src_string, uint32_t src_size, int32_t src_cp, void *dst_string, uint32_t &dst_size, int32_t dst_cp)
{
    return Call_Function<int32_t, const void *, uint32_t, int32_t, void *, uint32_t &, int32_t>(
        0x00411C25, src_string, src_size, src_cp, dst_string, dst_size, dst_cp);
}

void GenConvertString(FILE *fp)
{
    wchar_t wide_buff[512];
    char narrow_buff[1024];
    uint32_t wide_size = sizeof(wide_buff);
    uint32_t narrow_size = sizeof(narrow_buff);
    size_t string_size;

    const char test_greek_utf8[] = u8"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο";
    const char test_thai_utf8[] =
        u8"๏ เป็นมนุษย์สุดประเสริฐเลิศคุณค่า  กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า "
        u8"หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะๆจ๋าๆ น่าฟังเอย ฯ ";
    const char test_cyrillic_utf8[] = u8"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!";
    const char test_japanese_utf8[] =
        u8"いろはにほへとちりぬるを わかよたれそつねならむ うゐのおくやまけふこえてあさきゆめみしゑひもせす";

    const wchar_t test_greek_utf16[] = L"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο";
    const wchar_t test_thai_utf16[] =
        L"๏ เป็นมนุษย์สุดประเสริฐเลิศคุณค่า  กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า "
        L"หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะๆจ๋าๆ น่าฟังเอย ฯ ";
    const wchar_t test_cyrillic_utf16[] = L"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!";
    const wchar_t test_japanese_utf16[] =
        L"いろはにほへとちりぬるを わかよたれそつねならむ うゐのおくやまけふこえてあさきゆめみしゑひもせす";

    std::fprintf(fp, "/* ++++++ConvertStringEncoding test data here++++++ */\n");

    GenConvertStringEncoding(test_greek_utf8, sizeof(test_greek_utf8), 8, narrow_buff, narrow_size, 1253);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char greek_utf8_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_greek_utf16, sizeof(test_greek_utf16) / 2, 16, narrow_buff, narrow_size, 1253);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char greek_utf16_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_thai_utf8, sizeof(test_thai_utf8), 8, narrow_buff, narrow_size, 874);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char thai_utf8_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_thai_utf16, sizeof(test_thai_utf16) / 2, 16, narrow_buff, narrow_size, 874);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char thai_utf16_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_cyrillic_utf8, sizeof(test_cyrillic_utf8), 8, narrow_buff, narrow_size, 1251);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char cyrillic_utf8_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_cyrillic_utf16, sizeof(test_cyrillic_utf16) / 2, 16, narrow_buff, narrow_size, 1251);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char cyrillic_utf16_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_japanese_utf8, sizeof(test_japanese_utf8), 8, narrow_buff, narrow_size, 932);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char japanese_utf8_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");

    narrow_size = sizeof(narrow_buff);
    GenConvertStringEncoding(test_japanese_utf16, sizeof(test_japanese_utf16) / 2, 16, narrow_buff, narrow_size, 932);
    string_size = strlen(narrow_buff);
    std::fprintf(fp, "const char japanese_utf16_cp[] = \"");
    for (unsigned i = 0; i < string_size; ++i) {
        std::fprintf(fp, "\\x%02x", uint8_t(narrow_buff[i]));
    }
    std::fprintf(fp, "\";\n");
}
