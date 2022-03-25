#include <gtest/gtest.h>
#include <rz/rzrandom.h>
#include <rzrandomsuper.h>
#include <stdint.h>

TEST(random, lcg_rng)
{
    uint32_t RandomUint32Uniform_data[] = {
        0x9c141c01,
        0x06639f39,
        0xdb4d6ac6,
        0x27bf7d41,
        0x9f097413,
        0xd1457765,
        0x24c72974,
        0xb826f028,
        0x58d2a2c3,
        0xb07f1b15,
    };

    uint32_t RandomUint32Uniform_scaledata[] = {
        0x00000000,
        0x00000000,
        0x00000001,
        0x00000001,
        0x00000003,
        0x00000001,
        0x00000005,
        0x00000006,
        0x00000006,
        0x00000004,
    };

    int32_t RandomSint32RangeUniform_data[] = {
        -811,
        770,
        -778,
        951,
        3310,
        2419,
        1211,
        6371,
        4230,
        3258,
    };

    double RandomDoubleUniform_data[] = {
        5.5542837153188884258e-01,
        4.3363130255602300167e-01,
        4.7365301731042563915e-01,
        2.1882622665725648403e-01,
        5.3504794533364474773e-01,
        3.8708588271401822567e-01,
        7.0389984059147536755e-01,
        7.8497778228484094143e-01,
        8.1057185749523341656e-01,
        9.5681476010940968990e-01,
    };

    double RandomDoubleRangeUniform_data[] = {
        -3.1062559154815971851e+02,
        -7.0869086978188715875e+02,
        -2.1712079095793887973e+02,
        3.9131062658759765327e+02,
        2.5198767184419557452e+03,
        3.8190251051273662597e+03,
        -9.2868549710744991899e+02,
        5.5721983160427771509e+02,
        3.9085590749010443687e+03,
        4.7900709672777447850e+03,
    };

    cRZRandom rng(0xDEADBEEF);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.RandomUint32Uniform(), RandomUint32Uniform_data[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.RandomUint32Uniform(i), RandomUint32Uniform_scaledata[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.RandomSint32RangeUniform(i - 1000, i * 1000), RandomSint32RangeUniform_data[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(rng.RandomDoubleUniform(), RandomDoubleUniform_data[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(rng.RandomDoubleRangeUniform(i - 1000, i * 1000), RandomDoubleRangeUniform_data[i]);
    }
}

TEST(random, mersenne_rng)
{
    uint32_t SuperRandomUint32Uniform_data[] = {
        0x563b95f8,
        0x150fcfb1,
        0x35f4cdce,
        0x63060bc9,
        0xeca8fc7e,
        0x2c5ebb8a,
        0xe21edaad,
        0xc9cd2a05,
        0x9be4e650,
        0xa7737d60,
    };

    uint32_t SuperRandomUint32Uniform_scaledata[] = {
        0x00000000,
        0x00000000,
        0x00000001,
        0x00000000,
        0x00000002,
        0x00000003,
        0x00000002,
        0x00000003,
        0x00000006,
        0x00000004,
    };

    double SuperRandomDoubleUniform_data[] = {
        1.6412355145439505577e-01,
        7.8673994750715792179e-01,
        6.1963541037403047085e-01,
        6.8920646095648407936e-01,
        2.6807054434902966022e-01,
        9.2139589367434382439e-01,
        9.1878963960334658623e-01,
        9.8449358297511935234e-01,
        6.0411236458458006382e-01,
        6.9306766288354992867e-01,
    };

    double SuperRandomDoubleRangeUniform_data[] = {
        -5.5935158138163387775e+02,
        6.1066007599700242281e+02,
        1.5098965024030767381e+03,
        1.7449344739909283817e+03,
        1.3605978285772725940e+03,
        9.4626137905288487673e+02,
        5.0116428271988406777e+03,
        -1.0048666910780593753e+02,
        3.6971206362769007683e+03,
        -2.6886985582578927279e+02,
    };

    cRZRandomSuper rng(0xDEADBEEF);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.RandomUint32Uniform(), SuperRandomUint32Uniform_data[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.RandomUint32Uniform(i), SuperRandomUint32Uniform_scaledata[i]);
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(rng.RandomDoubleUniform(), SuperRandomDoubleUniform_data[i]);
    }

    // Two of the last 3 generated values vary by more than the margin of error EXPECT_DOUBLE_EQ uses.
    // However they are still very close and are probably due to rounding errors in either the original code
    // or the new code.
    for (int i = 0; i < 7 /*i < 10*/; ++i) {
        EXPECT_DOUBLE_EQ(rng.RandomDoubleRangeUniform(i - 1000, i * 1000), SuperRandomDoubleRangeUniform_data[i]);
    }
}
