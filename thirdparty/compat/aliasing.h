/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Types that won't trigger strict aliasing.
 *
 * @copyright Maverick is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <stdint.h>

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

// Use these when using pointers to do type punning.
#if __has_attribute(__may_alias__)
typedef float __attribute__((__may_alias__)) float_alias_t;
typedef int32_t __attribute__((__may_alias__)) int32_alias_t;
typedef uint32_t __attribute__((__may_alias__)) uint32_alias_t;
typedef double __attribute__((__may_alias__)) double_alias_t;
typedef int64_t __attribute__((__may_alias__)) int64_alias_t;
typedef uint64_t __attribute__((__may_alias__)) uint64_alias_t;
#else
// MSVC doesn't currently enforce strict aliasing.
typedef float float_alias_t;
typedef int32_t int32_alias_t;
typedef uint32_t uint32_alias_t;
typedef double double_alias_t;
typedef int64_t int64_alias_t;
typedef uint64_t uint64_alias_t;
#endif

// Union to use for type punning floats.
union float_int_pun_t
{
    uint32_alias_t integer;
    float_alias_t real;
};
