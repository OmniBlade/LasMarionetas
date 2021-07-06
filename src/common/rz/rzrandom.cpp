/**
 * @file
 *
 * @brief A Linear congruential pseudo random number generator.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzrandom.h"
#include <ctime>

cRZRandom::cRZRandom(uint32_t seed)
{
    Seed(seed);
    mnUnknown = 0;
}

void cRZRandom::Seed(uint32_t seed)
{
    if (seed == UINT32_MAX) {
        clock_t clock_ticks = std::clock();
        seed = uint32_t(std::time(nullptr)) ^ (clock_ticks | (clock_ticks << 16));
    }

    mnIntegerSeed = seed;
    mnRealSeed = seed;
}

uint32_t cRZRandom::RandomUint32Uniform()
{
    uint64_t tmp = 1103515245ll *mnIntegerSeed + 12345;
    mnIntegerSeed = uint32_t(tmp);

    return uint32_t(tmp >> 16);
}

uint32_t cRZRandom::RandomUint32Uniform(uint32_t scale)
{
    return (scale * uint64_t(RandomUint32Uniform())) >> 32;
}

int32_t cRZRandom::RandomSint32RangeUniform(int32_t min, int32_t max)
{
    return min + RandomUint32Uniform(max - min);
}

double cRZRandom::RandomDoubleUniform()
{
    if (mnRealSeed == 0) {
        mnRealSeed = std::clock();
    }

    mnRealSeed = 663608941 * mnRealSeed;

    return mnRealSeed * 2.328306436538696e-10 + 0.5;
}

double cRZRandom::RandomDoubleRangeUniform(double min, double max)
{
    return RandomDoubleUniform() * (max - min) + min;
}
