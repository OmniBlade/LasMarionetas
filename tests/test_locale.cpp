#include <gtest/gtest.h>
#include <rz/rzlocale.h>

#ifdef _WIN32
#include <windows.h>
#include <winnls.h>
#endif

TEST(locale, codepage)
{
#ifdef _WIN32
    EXPECT_EQ(GetCurrentSystemCodePage(), GetACP());
#else
    EXPECT_EQ(GetCurrentSystemCodePage(), kUTF8CodePage);
#endif
}
