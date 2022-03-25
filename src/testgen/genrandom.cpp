#include "genrandom.h"
#include <hooker.h>
#include <stdint.h>

class cGameRandom
{
public:
    cGameRandom(uint32_t seed = UINT32_MAX) { Call_Method<void, cGameRandom, uint32_t>(0x0041821F, this, seed); }
    void Seed(uint32_t seed) { Call_Method<void, cGameRandom, uint32_t>(0x00417F71, this, seed); }
    uint32_t RandomUint32Uniform() { return Call_Method<uint32_t, cGameRandom>(0x00417FA0, this); }
    uint32_t RandomUint32Uniform(uint32_t scale)
    {
        return Call_Method<uint32_t, cGameRandom, uint32_t>(0x00417FBB, this, scale);
    }
    int32_t RandomSint32RangeUniform(int32_t min, int32_t max)
    {
        return Call_Method<int32_t, cGameRandom, int32_t, int32_t>(0x00417FD4, this, min, max);
    }
    double RandomDoubleUniform() { return Call_Method<double, cGameRandom>(0x00418038, this); }
    double RandomDoubleRangeUniform(double min, double max)
    {
        return Call_Method<double, cGameRandom, double, double>(0x00041806B, this, min, max);
    }

private:
    uint32_t mnIntegerSeed;
    int32_t mnRealSeed;
    int32_t mnUnknown;
};

void GenRandom(FILE *fp)
{
    cGameRandom rng(0xDEADBEEF);
    std::fprintf(fp, "/* ++++++cRZRandom test data here++++++ */\n");
    std::fprintf(fp, "uint32_t RandomUint32Uniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    0x%08x,\n", rng.RandomUint32Uniform());
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "uint32_t RandomUint32Uniform_scaledata[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    0x%08x,\n", rng.RandomUint32Uniform(i));
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "int32_t RandomSint32RangeUniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    %d,\n", rng.RandomSint32RangeUniform(i - 1000, i * 1000));
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "double RandomDoubleUniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    %1.19e,\n", rng.RandomDoubleUniform());
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "double RandomDoubleRangeUniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    %1.19e,\n", rng.RandomDoubleRangeUniform(i - 1000, i * 1000));
    }
    std::fprintf(fp, "};\n\n");
}

class cGameRandomSuper
{
    enum
    {
        TWIST_SIZE = 624,
    };

public:
    cGameRandomSuper(uint32_t seed = UINT32_MAX) { Call_Method<void, cGameRandomSuper, uint32_t>(0x0041829B, this, seed); }
    void Seed(uint32_t seed) { Call_Method<void, cGameRandomSuper, uint32_t>(0x0041823A, this, seed); }
    uint32_t RandomUint32Uniform() { return Call_Method<uint32_t, cGameRandomSuper>(0x00418191, this); }
    uint32_t RandomUint32Uniform(uint32_t scale)
    {
        return Call_Method<uint32_t, cGameRandomSuper, uint32_t>(0x004181DE, this, scale);
    }
    double RandomDoubleUniform() { return Call_Method<double, cGameRandomSuper>(0x004181EC, this); }
    double RandomDoubleRangeUniform(double min, double max)
    {
        return Call_Method<double, cGameRandomSuper, double, double>(0x00418209, this, min, max);
    }

private:
    uint32_t mTwister[TWIST_SIZE];
    uint32_t *mpNext;
    int32_t mnRemaining;
};

void GenSuperRandom(FILE *fp)
{
    cGameRandomSuper rng(0xDEADBEEF);
    std::fprintf(fp, "/* ++++++cRZRandomSuper test data here++++++ */\n");
    std::fprintf(fp, "uint32_t SuperRandomUint32Uniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    0x%08x,\n", rng.RandomUint32Uniform());
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "uint32_t SuperRandomUint32Uniform_scaledata[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    0x%08x,\n", rng.RandomUint32Uniform(i));
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "double SuperRandomDoubleUniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    %1.19e,\n", rng.RandomDoubleUniform());
    }
    std::fprintf(fp, "};\n\n");

    std::fprintf(fp, "double SuperRandomDoubleRangeUniform_data[] = {\n");
    for (int i = 0; i < 10; ++i) {
        std::fprintf(fp, "    %1.19e,\n", rng.RandomDoubleRangeUniform(i - 1000, i * 1000));
    }
    std::fprintf(fp, "};\n\n");
}
