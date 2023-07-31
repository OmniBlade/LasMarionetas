#include "test_common.h"
#include <gtest/gtest.h>
#include <rzstring.h>
#include <rzversion.h>

TEST(version, sys_version)
{
    cRZString test;
    cRZVersion version;
    EXPECT_TRUE(version.GetSystemVersion(test));
    EXPECT_FALSE(test.IsEmpty());
    std::cout << GTEST_BOX << "Operating System = " << test.c_str() << std::endl;
}

TEST(version, cpu_speed)
{
    uint32_t speed;
    cRZVersion version;
    speed = version.GetCPUSpeed();
    EXPECT_GT(speed, 0);
    std::cout << GTEST_BOX << "CPU Speed = " << speed << " Mhz" << std::endl;
}

TEST(version, cpu_name)
{
    cRZString test;
    cRZVersion version;
    EXPECT_TRUE(version.GetCPUName(test));
    EXPECT_FALSE(test.IsEmpty());
    std::cout << GTEST_BOX << "CPU Name = " << test.c_str() << std::endl;
}

TEST(version, sys_mem)
{
    size_t total = 0;
    size_t free = 0;
    cRZVersion version;
    EXPECT_TRUE(version.GetSystemMemory(total, free));
    EXPECT_GT(total, 0);
    EXPECT_GT(free, 0);
    std::cout << GTEST_BOX << "Total memory = " << total << " MiB" << std::endl;
    std::cout << GTEST_BOX << "Free memory = " << free << " MiB" << std::endl;
}

TEST(version, user_name)
{
    cRZString test;
    cRZVersion version;
    EXPECT_TRUE(version.GetUserName(test));
    EXPECT_FALSE(test.IsEmpty());
    std::cout << GTEST_BOX << "User Name = " << test.c_str() << std::endl;
}

TEST(version, host_name)
{
    cRZString test;
    cRZVersion version;
    EXPECT_TRUE(version.GetHostName(test));
    EXPECT_FALSE(test.IsEmpty());
    std::cout << GTEST_BOX << "User Name = " << test.c_str() << std::endl;
}
