/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Mask differences between win32 and icu UTF-16 API.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#ifdef BUILD_WITH_ICU
#include <unicode/uchar.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>
typedef UChar unichar_t;
#define U_CHAR(str) (u##str)
#elif defined _WIN32
#include <stdio.h>
#include <wchar.h>
typedef wchar_t unichar_t;
#define U_CHAR(str) (L##str)
#define u_strlen wcslen
#define u_strcpy wcscpy
#define u_strcat wcscat
#define u_vsnprintf_u(w, x, y, z) vswprintf(w, x, y, z)
#define u_strcmp wcscmp
#define u_strcasecmp(x, y, z) _wcsicmp(x, y)
#define u_isspace iswspace
#define u_tolower towlower
#define u_strchr wcschr
#define U_COMPARE_CODE_POINT_ORDER 0x8000
#else
#error Support for utf16<->utf8 conversion not found for this platform.
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(__may_alias__)
typedef unichar_t __attribute__((__may_alias__)) unichar_alias_t;
#else
typedef unichar_t unichar_alias_t;
#endif
