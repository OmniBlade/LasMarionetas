#include <codex.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <stdio.h>
#include <vector>

TEST(codex, refpack)
{
    struct stat st;
    int ret = stat(TESTDATA_PATH "/cantebury/kennedy.xls", &st);

    if (ret != 0) {
        printf("Failed to find test data.\n");
        return;
    }

    FILE *fp = fopen(TESTDATA_PATH "/cantebury/kennedy.xls", "rb");

    if (fp == nullptr) {
        printf("Failed to open test data.\n");
        return;
    }

    std::vector<uint8_t> src(st.st_size);
    std::vector<uint8_t> cmp(st.st_size);
    std::vector<uint8_t> dec(st.st_size);

    // Test data round trips successfully.
    fread(src.data(), 1, st.st_size, fp);
    REF_encode(cmp.data(), src.data(), st.st_size);

    int dec_size;
    REF_decode(dec.data(), cmp.data(), &dec_size);
    EXPECT_EQ(dec_size, st.st_size);
    EXPECT_TRUE(src == dec);
}
