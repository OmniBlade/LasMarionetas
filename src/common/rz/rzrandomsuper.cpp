/**
 * @file
 *
 * @brief A mersenne twister pseudo random number generator.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzrandomsuper.h"
#include <ctime>

cRZRandomSuper::cRZRandomSuper(uint32_t seed) : mpNext(nullptr), mnRemaining(624)
{
    Seed(seed);
}

void cRZRandomSuper::Seed(uint32_t seed)
{
    // Generate a seed if one isn't provided.
    if (seed == UINT32_MAX) {
        seed = Hash(int32_t(std::time(nullptr)), int32_t(std::clock()));
    }

    // Make the seed odd if it isn't already.
    seed |= 1;

    // Seed the twister values using an LCG algo.
    for (int i = 0; i < TWIST_SIZE; ++i) {
        mTwister[i] = ((69069 * seed) >> 16) | (seed & 0xFFFF0000);
        seed = 69069 * (69069 * seed + 1) + 1;
    }

    Reload();
}

uint32_t cRZRandomSuper::RandomUint32Uniform()
{
    --mnRemaining;

    // We are out of twisted values, retwist the array.
    if (mnRemaining < 0) {
        Reload();
        --mnRemaining;
    }

    uint32_t y = *mpNext++;
    y ^= y >> TEMPERING_U;
    y ^= (y << TEMPERING_S) & TEMPERING_B;
    y ^= (y << TEMPERING_T) & TEMPERING_C;
    y ^= y >> TEMPERING_L;
    // This last tempering step does not seem to be in the original reference implementation
    y ^= (y << TEMPERING_T) & TEMPERING_C;

    return y;
}

uint32_t cRZRandomSuper::RandomUint32Uniform(uint32_t scale)
{
    return (scale * uint64_t(RandomUint32Uniform())) >> 32;
}

int32_t cRZRandomSuper::RandomSint32RangeUniform(int32_t min, int32_t max)
{
    return min + RandomUint32Uniform(max - min);
}

double cRZRandomSuper::RandomDoubleUniform()
{
    return RandomUint32Uniform() * 2.328306436538696e-10 + 0.5;
}

double cRZRandomSuper::RandomDoubleRangeUniform(double min, double max)
{
    return RandomDoubleUniform() * (max - min) + min;
}

void cRZRandomSuper::Reload()
{
    // The loop is unrolled into 3 parts to avoid the need for a modulo operation.
    for (int i = 0; i < TWIST_DIFF; ++i) {
        uint32_t y = (mTwister[i] & 0x80000000) | (mTwister[i + 1] & 0x7FFFFFFF);
        mTwister[i] = mTwister[i + TWIST_PERIOD] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & XOR_MASK);
    }

    for (int i = TWIST_DIFF; i < TWIST_SIZE; ++i) {
        uint32_t y = (mTwister[i] & 0x80000000) | (mTwister[i + 1] & 0x7FFFFFFF);
        mTwister[i] = mTwister[i - TWIST_DIFF] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & XOR_MASK);
    }

    uint32_t y = (mTwister[TWIST_SIZE - 1] & 0x80000000) | (mTwister[0] & 0x7FFFFFFF);
    mTwister[TWIST_SIZE - 1] = mTwister[TWIST_PERIOD - 1] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & XOR_MASK);
    mnRemaining = TWIST_SIZE;
    mpNext = mTwister;
}

uint32_t cRZRandomSuper::Hash(int32_t val1, int32_t val2)
{
    static int _incrementor;
    uint8_t byte1[sizeof(val1)];
    uint8_t byte2[sizeof(val2)];

    *reinterpret_cast<int32_t *>(byte1) = val1;
    *reinterpret_cast<int32_t *>(byte2) = val2;

    int32_t hash1 = 0;

    for (unsigned i = 0; i < sizeof(byte1); ++i) {
        hash1 = 257 * hash1 + byte1[i];
    }
    
    int32_t hash2 = 0;

    for (unsigned i = 0; i < sizeof(byte2); ++i) {
        hash2 = 257 * hash2 + byte2[i];
    }

    return hash2 ^ (hash1 + _incrementor++);
}
