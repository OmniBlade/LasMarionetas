#include <gtest/gtest.h>
#include <rzsleep.h>
#include <rztimer.h>

TEST(timer, running)
{
    cRZTimer test;
    test.Start();
    EXPECT_TRUE(test.IsTimerRunning());
}

TEST(timer, elapsed)
{
    cRZTimer sec(cRZTimer::kSeconds);
    cRZTimer mili(cRZTimer::kMilliseconds);
    cRZTimer micro(cRZTimer::kMicroseconds);
    cRZTimer nano(cRZTimer::kNanoSeconds);
    cRZTimer high(cRZTimer::kHighResolutionTimer);
    cRZTimer cpu(cRZTimer::kProcessorTicks);

    sec.Start();
    mili.Start();
    micro.Start();
    nano.Start();
    high.Start();
    cpu.Start();

    // Sleep 1 second to allow all our timers to tick at least once.
    RZSleep(1000);

    EXPECT_GE(sec.GetElapsedTime(), 1);
    EXPECT_GE(mili.GetElapsedTime(), 1000);
    EXPECT_GE(micro.GetElapsedTime(), 1000000);
    EXPECT_GE(nano.GetElapsedTime(), 1000000000);
    EXPECT_GE(high.GetElapsedTime(), 1000000000);
    EXPECT_GE(cpu.GetElapsedTime(), 1000000000);
}

TEST(timer, set)
{
    cRZTimer sec(cRZTimer::kSeconds);
    cRZTimer mili(cRZTimer::kMilliseconds);
    cRZTimer micro(cRZTimer::kMicroseconds);
    cRZTimer nano(cRZTimer::kNanoSeconds);
    cRZTimer high(cRZTimer::kHighResolutionTimer);
    cRZTimer cpu(cRZTimer::kProcessorTicks);

    // Set to 10 seconds and then immediately test to ensure set correctly.
    sec.SetElapsedTime(10);
    mili.SetElapsedTime(10000);
    micro.SetElapsedTime(10000000);
    nano.SetElapsedTime(10000000000);
    high.SetElapsedTime(10000000000);
    cpu.SetElapsedTime(10000000000);

    EXPECT_GE(sec.GetElapsedTime(), 10);
    EXPECT_GE(mili.GetElapsedTime(), 10000);
    EXPECT_GE(micro.GetElapsedTime(), 10000000);
    EXPECT_GE(nano.GetElapsedTime(), 10000000000);
    EXPECT_GE(high.GetElapsedTime(), 10000000000);
    EXPECT_GE(cpu.GetElapsedTime(), 10000000000);
}

TEST(timer, change_units)
{
    // First test the timer ticks at least once after a sleep of 1 ms.
    cRZTimer test(cRZTimer::kMilliseconds);
    test.Start();
    RZSleep(1);
    EXPECT_GE(test.GetElapsedTime(), 1);

    // Now check it ticked at least 1000 us after a 1 ms sleep.
    test.SetUnits(cRZTimer::kMicroseconds);
    test.Start();
    RZSleep(1);
    EXPECT_GE(test.GetElapsedTime(), 1000);
}
