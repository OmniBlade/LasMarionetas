/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Bitwise rotate functions that utilise intrinsics where available.
 *
 * @copyright Compat is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#ifndef STDCOMPAT_ROTATE_H
#define STDCOMPAT_ROTATE_H

#include <stdint.h>

/* When available, these headers can improve platform endianness detection */
#ifdef __has_include // C++17, supported as extension to C++11 in clang, GCC 5+, vs2015
#if __has_include(<intrin.h>)
#include <intrin.h> // MSVC, intrinsics for all supported architectures.
#elif __has_include(<x86intrin.h>) // Clang/GCC x86 intrinsics.
#include <x86intrin.h> //open bsd, macos
#endif
#endif

/* Temp macro for intrinsic inlining */
#if defined __GNUC__ || defined __clang__
#define __intrin_inline extern __attribute__((gnu_inline, always_inline)) inline
#else
#define __intrin_inline inline
#endif

/* These are fallbacks that should generate the correct instructions where available */
__intrin_inline uint8_t __rotl8(uint8_t a, unsigned b)
{
    b &= 7;
    return (a << b) | (a >> (8 - b));
}

__intrin_inline uint8_t __rotr8(uint8_t a, unsigned b)
{
    b &= 7;
    return (a >> b) | (a << (8 - b));
}

__intrin_inline uint16_t __rotl16(uint16_t a, unsigned b)
{
    b &= 15;
    return (a << b) | (a >> (16 - b));
}

__intrin_inline uint16_t __rotr16(uint16_t a, unsigned b)
{
    b &= 15;
    return (a >> b) | (a << (16 - b));
}

__intrin_inline uint32_t __rotl32(uint32_t a, unsigned b)
{
    b &= 31;
    return (a << b) | (a >> (32 - b));
}

__intrin_inline uint32_t __rotr32(uint32_t a, unsigned b)
{
    b &= 31;
    return (a >> b) | (a << (32 - b));
}

__intrin_inline uint64_t __rotl64(uint64_t a, unsigned b)
{
    b &= 63;
    return (a << b) | (a >> (64 - b));
}

__intrin_inline uint64_t __rotr64(uint64_t a, unsigned b)
{
    b &= 63;
    return (a >> b) | (a << (64 - b));
}

#if defined _MSC_VER // MSVC compiler.
#define rotl8(a, b) _rotl8(a, b)
#define rotr8(a, b) _rotr8(a, b)
#define rotl16(a, b) _rotl16(a, b)
#define rotr16(a, b) _rotr16(a, b)
#define rotl32(a, b) _rotl(a, b)
#define rotr32(a, b) _rotr(a, b)
#define rotl64(a, b) _rotl64(a, b)
#define rotr64(a, b) _rotr64(a, b)
#elif (defined __GNUC__ || defined __clang__) && (defined __i386__ || defined __x86_64__)
#define rotl8(a, b) __rolb(a, b)
#define rotr8(a, b) __rorb(a, b)
#define rotl16(a, b) __rolw(a, b)
#define rotr16(a, b) __rorw(a, b)
#define rotl32(a, b) __rold(a, b)
#define rotr32(a, b) __rord(a, b)
#if defined __x86_64__
#define rotl64(a, b) __rolq(a, b)
#define rotr64(a, b) __rorq(a, b)
#else
#define rotl64(a, b) __rotl64(a, b)
#define rotr64(a, b) __rotr64(a, b)
#endif
#else
#define rotl8(a, b) __rotl8(a, b)
#define rotr8(a, b) __rotr8(a, b)
#define rotl16(a, b) __rotl16(a, b)
#define rotr16(a, b) __rotr16(a, b)
#define rotl32(a, b) __rotl32(a, b)
#define rotr32(a, b) __rotr32(a, b)
#define rotl64(a, b) __rotl64(a, b)
#define rotr64(a, b) __rotr64(a, b)
#endif

#endif /* STDCOMPAT_ROTATE_H */
