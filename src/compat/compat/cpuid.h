/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Masks differences between compilers cpuid intrinsic.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */

#ifndef STDCOMPAT_CPUID_H
#define STDCOMPAT_CPUID_H

#if defined _WIN32 && (defined _M_IX86 || defined _M_AMD64)
#include <intrin.h>
#define cpuid(regs, cpuid_type) __cpuid(reinterpret_cast<int *>(regs), cpuid_type)
#define cpuidex(regs, cpuid_type, count) __cpuidex(reinterpret_cast<int *>(regs), cpuid_type, count)
#elif (defined __clang__ || defined __GNUC__) && (defined __i386__ || defined __amd64__)
#include_next <cpuid.h>
#define cpuid(regs, cpuid_type) __cpuid(cpuid_type, regs[0], regs[1], regs[2], regs[3])
#define cpuidex(regs, cpuid_type, count) __cpuid_count(cpuid_type, count, regs[0], regs[1], regs[2], regs[3])
#else
/* Just return 0 for everything if its not x86 */
#include <string.h>
#define cpuid(regs, cpuid_type) memset(regs, 0, 16)
#define cpuidex(regs, cpuid_type, count) memset(regs, 0, 16)
#endif

#endif /* STDCOMPAT_CPUID_H */
