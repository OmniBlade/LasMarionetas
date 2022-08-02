/**
 * @file
 *
 * @brief CODEX public header.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <endianness.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

/* Original binary uses __stdcall so need to emulate that for msvc 32bit builds */
#if defined _MSC_VER && defined _M_IX86
#define CODEX_API __stdcall
#else
#define CODEX_API
#endif

bool CODEX_API REF_is(const void *compresseddata);
int CODEX_API REF_decode(void *dest, const void *compresseddata, int *compressedsize);
int CODEX_API REF_encode(void *compresseddata, const void *source, int sourcesize);

#ifdef __cplusplus
} // extern "C"
#endif
