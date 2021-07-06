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
#pragma once

#include <stdint.h>

class cRZRandomSuper
{
    enum
    {
        TWIST_SIZE = 624,
        TWIST_PERIOD = 397,
        TWIST_DIFF = TWIST_SIZE - TWIST_PERIOD,
        TEMPERING_U = 11,
        TEMPERING_S = 7,
        TEMPERING_B = 0x9D2C5680,
        TEMPERING_T = 15,
        TEMPERING_C = 0xEFC60000,
        TEMPERING_L = 18,
        XOR_MASK = 0x9908B0DF,
    };

public:
    cRZRandomSuper(uint32_t seed = UINT32_MAX);

    /**
     * @brief Seeds the random number generator.
     * @param seed Value to use to seed the generator.
     */
    void Seed(uint32_t seed);
    /**
     * @brief Generates a random 32bit unsigned integer.
     * @return Random value.
     */
    uint32_t RandomUint32Uniform();
    /**
     * @brief Generates a random 32bit unsigned integer.
     * @param scale Value returned should be no larger than this.
     * @return Random value.
     */
    uint32_t RandomUint32Uniform(uint32_t scale);
    /**
     * @brief Generates a random 32bit unsigned integer within a range.
     * @param min Value returned should be no smaller than this.
     * @param max Value returned should be no larger than this.
     * @return Random value.
     */
    int32_t RandomSint32RangeUniform(int32_t min, int32_t max);
    /**
     * @brief Generates a random 64bit floating point value.
     * @return Random value.
     */
    double RandomDoubleUniform();
    /**
     * @brief Generates a random 64bit floating point value.
     * @param min Value returned should be no smaller than this.
     * @param max Value returned should be no larger than this.
     * @return Random value.
     */
    double RandomDoubleRangeUniform(double min, double max);

private:
    void Reload();
    uint32_t Hash(int32_t val1, int32_t val2);

private:
    uint32_t mTwister[TWIST_SIZE];
    uint32_t *mpNext;
    int32_t mnRemaining;
};
