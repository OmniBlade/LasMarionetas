/**
 * @file
 *
 * @brief Wrappers to set and get locale information.
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

enum CodePageEnum
{
    kUndefinedCodePage = -3,
    kUnicode = -2,
    kSystemCodePage = -1,
    kDefaultCodePage = 0,
    kUTF8CodePage = 8,
    kUTF16CodePage = 16,
    kUTF32CodePage = 32,
    kICUUTF8CodePage = 1208,
    kWinUTF7CodePage = 65000,
    kWinUTF8CodePage = 65001,
};

/**
 * Wrapper around setlocale that also clears the internal record of the current code page.
 */
char *RZSetlocaleA(int category, char const *locale);
/**
 * @return The current system code page.
 */
int32_t GetCurrentSystemCodePage();
/**
 * @return The current framework code page, always UTF-8.
 */
int32_t GetCurrentFrameworkCodePage();
