/**
 * @file
 *
 * @brief String code page conversion utility functions.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "convertstring.h"
#include "igzstring.h"
#include "rzfastalloc.h"
#include "rzlocale.h"
#include "unichar.h"
#include <algorithm>
#include <cstring>
#include <vector>

#ifndef BUILD_WITH_ICU
#include <windows.h>
#endif

/**
 * Util function to convert some none specific code pages to a specific code page.
 */
static int32_t EffectiveCodePage(int32_t cp)
{
    switch (cp) {
        case kUnicode:
            return kUTF8CodePage;
        case kSystemCodePage:
            return GetCurrentSystemCodePage();
        default:
            break;
    };

    return cp;
}

/**
 * Util to check if a string is just ASCII.
 */
static bool IsAscii(const char *str, uint32_t str_size)
{
    for (unsigned i = 0; i < str_size; ++i) {
        if (str[i] & 0x80) {
            return false;
        }
    }

    return true;
}

/**
 * Util to copy strings of matching encoding.
 */
static uint32_t CopyString(const void *src_string, uint32_t &src_size, void *dst_string, uint32_t &dst_size, int32_t cp)
{
    if (src_string != dst_string) {
        src_size = std::min(src_size, dst_size);
        if (cp == kUTF16CodePage) {
            std::memcpy(dst_string, src_string, 2 * src_size);
        } else {
            std::memcpy(dst_string, src_string, src_size);
        }
    }

    dst_size = src_size;

    return src_size;
}

/**
 * Util to convert code pages going via UTF16.
 */
static int32_t ConvertStringEncoding_via_UTF16(
    const void *src_string, uint32_t src_size, int32_t src_cp, void *dst_string, uint32_t &dst_size, int32_t dst_cp)
{
    static constexpr uint32_t STACK_BUFF_SIZE = 1024;
    uint32_t buffer_size = 2 * src_size + 4;
    std::vector<char, cRZFastSTLAllocator<char>> vector_buffer;
    char stack_buffer[STACK_BUFF_SIZE];
    char *buffer_string = stack_buffer;

    if (buffer_size > STACK_BUFF_SIZE) {
        vector_buffer.resize(buffer_size);
        buffer_string = &vector_buffer[0];
    }

    if (ConvertStringEncoding(src_string, src_size, src_cp, buffer_string, buffer_size, kUTF16CodePage) != src_size
        || ConvertStringEncoding(buffer_string, buffer_size, kUTF16CodePage, dst_string, dst_size, dst_cp) != buffer_size) {
        dst_size = 0;
        return 0;
    }

    return src_size;
}

int32_t ConvertStringEncoding(
    const void *src_string, uint32_t src_size, int32_t src_cp, void *dst_string, uint32_t &dst_size, int32_t dst_cp)
{
    if (src_size == 0) {
        dst_size = 0;
        return 0;
    }

    int32_t effective_src_cp =
        !IsAscii(static_cast<const char *>(src_string), src_size) ? EffectiveCodePage(src_cp) : kDefaultCodePage;
    int32_t effective_dst_cp = EffectiveCodePage(dst_cp);

    // If the strings are compatible with a straight copy, then do that.
    if (effective_src_cp == effective_dst_cp || (effective_src_cp == kDefaultCodePage && effective_dst_cp != kUTF16CodePage)
        || (effective_src_cp != kUTF16CodePage && effective_dst_cp == kDefaultCodePage)) {
        return CopyString(src_string, src_size, dst_string, dst_size, effective_dst_cp);
    }

    // Treat the default code page as though it were the english windows default.
    if (effective_dst_cp == kDefaultCodePage && effective_src_cp == kUTF16CodePage) {
        uint32_t size = std::min(src_size, dst_size);
        const wchar_t *getp = static_cast<const wchar_t *>(src_string);
        uint8_t *putp = static_cast<uint8_t *>(dst_string);
        const wchar_t *endp = getp + size;

        while (getp < endp) {
            *putp++ = uint8_t(*getp++);
        }

        dst_size = size;
        return size;
    }

    // If we have the same pointers for source and destination then do nothing.
    if (src_string == dst_string) {
        return 0;
    }
#ifdef BUILD_WITH_ICU
    // If we have UTF-16 then we can use conversion functions.
    if (effective_src_cp == kUTF16CodePage) {
        UErrorCode error;
        effective_dst_cp = effective_dst_cp != kUTF8CodePage ? effective_dst_cp : kICUUTF8CodePage;
        UConverter *conv = ucnv_openCCSID(effective_dst_cp, UCNV_IBM, &error);
        int length = ucnv_fromUChars(
            conv, static_cast<char *>(dst_string), dst_size, static_cast<const unichar_t *>(src_string), src_size, &error);
        ucnv_close(conv);

        if (length == 0) {
            dst_size = 0;
            return 0;
        }

        dst_size = length;
        return src_size;
    }

    // If we are going to UTF-16 then we can use conversion functions.
    if (effective_dst_cp == kUTF16CodePage) {
        UErrorCode error;
        effective_src_cp = effective_src_cp != kUTF8CodePage ? effective_src_cp : kICUUTF8CodePage;
        UConverter *conv = ucnv_openCCSID(effective_dst_cp, UCNV_IBM, &error);
        int length = ucnv_toUChars(
            conv, static_cast<unichar_t *>(dst_string), dst_size, static_cast<const char *>(src_string), src_size, &error);
        ucnv_close(conv);

        if (length == 0) {
            dst_size = 0;
            return 0;
        }

        dst_size = length;
        return src_size;
    }
#else
    // If we have UTF-16 then we can use conversion functions.
    if (effective_src_cp == kUTF16CodePage) {

        BOOL unmapped = FALSE;
        char defchar = ' ';
        effective_dst_cp = effective_dst_cp != kUTF8CodePage ? effective_dst_cp : kWinUTF8CodePage;
        int length = WideCharToMultiByte(effective_dst_cp,
            0,
            static_cast<const unichar_t *>(src_string),
            src_size,
            static_cast<char *>(dst_string),
            dst_size,
            &defchar,
            &unmapped);

        if (length == 0) {
            dst_size = 0;
            return 0;
        }

        dst_size = length;
        return src_size;
    }

    // If we are going to UTF-16 then we can use conversion functions.
    if (effective_dst_cp == kUTF16CodePage) {
        effective_src_cp = effective_src_cp != kUTF8CodePage ? effective_src_cp : kWinUTF8CodePage;
        int length = MultiByteToWideChar(effective_src_cp,
            0,
            static_cast<const char *>(src_string),
            src_size,
            static_cast<unichar_t *>(dst_string),
            dst_size);

        if (length == 0) {
            dst_size = 0;
            return 0;
        }

        dst_size = length;
        return src_size;
    }
#endif

    return ConvertStringEncoding_via_UTF16(src_string, src_size, effective_src_cp, dst_string, dst_size, effective_dst_cp);
}

int32_t ConvertStringEncoding(const void *src_string, uint32_t src_size, int32_t src_cp, cIGZString &dst_string)
{
    int32_t cp = EffectiveCodePage(src_cp);

    if (cp == kUTF8CodePage) {
        dst_string.FromChar(static_cast<const char *>(src_string), src_size);
    }

    uint32_t dst_size = 6 * src_size;
    char stack_buffer[512];
    char *dst_buffer = nullptr;
    char *heap_buffer = nullptr;

    if (dst_size < 512) {
        dst_buffer = stack_buffer;
    } else {
        heap_buffer = new char[dst_size];
        dst_buffer = heap_buffer;
    }

    int32_t result = 0;

    if (dst_buffer != nullptr) {
        dst_buffer[0] = '\0';
        result = ConvertStringEncoding(src_string, src_size, cp, dst_buffer, dst_size, kUTF8CodePage) == src_size;
        dst_string.FromChar(dst_buffer, dst_size);
    }

    delete[] heap_buffer;
    return result;
}

int32_t ConvertStringEncoding(const cIGZString &src_string, void *dst_string, uint32_t &dst_size, int32_t dst_cp)
{
    int32_t cp = EffectiveCodePage(dst_cp);

    if (cp == kUTF8CodePage) {
        dst_size = std::min(dst_size, src_string.Strlen());
        std::memcpy(dst_string, src_string.Data(), dst_size);

        return dst_size;
    }

    return ConvertStringEncoding(src_string.Data(), src_string.Strlen(), kUTF8CodePage, dst_string, dst_size, cp);
}

bool ConvertStringEncoding(const cIGZString &src_string, int32_t src_cp, cIGZString &dst_string)
{
    int32_t cp = EffectiveCodePage(src_cp);

    if (cp == kUTF8CodePage) {
        if (&src_string != &dst_string) {
            dst_string = src_string;
        }

        return true;
    }

    if (cp == kUTF16CodePage) {
        return false;
    }

    uint32_t src_size = src_string.Strlen();
    uint32_t dst_size = 6 * src_size;
    char stack_buffer[512];
    char *dst_buffer;
    char *heap_buffer = nullptr;

    if (dst_size < 512) {
        dst_buffer = stack_buffer;
    } else {
        heap_buffer = new char[dst_size];
        dst_buffer = heap_buffer;
    }

    bool result = false;

    if (dst_buffer != nullptr) {
        dst_buffer[0] = '\0';
        result = ConvertStringEncoding(src_string.Data(), src_size, cp, dst_buffer, dst_size, kUTF8CodePage) == src_size;
        dst_string.FromChar(dst_buffer, dst_size);
    }

    delete[] heap_buffer;
    return result;
}

bool ConvertStringEncoding(const cIGZString &src_string, cIGZString &dst_string, int32_t dst_cp)
{
    int32_t cp = EffectiveCodePage(dst_cp);

    if (cp == kUTF8CodePage) {
        if (&src_string != &dst_string) {
            dst_string = src_string;
        }

        return true;
    }

    uint32_t src_size = src_string.Strlen();
    uint32_t dst_size = 6 * src_size;
    char stack_buffer[512];
    char *dst_buffer;
    char *heap_buffer = nullptr;

    if (dst_size < 512) {
        dst_buffer = stack_buffer;
    } else {
        heap_buffer = new char[dst_size];
        dst_buffer = heap_buffer;
    }

    bool result = false;

    if (dst_buffer != nullptr) {
        dst_buffer[0] = '\0';
        result = ConvertStringEncoding(src_string.Data(), src_size, kUTF8CodePage, dst_buffer, dst_size, cp) == src_size;
        dst_string.FromChar(dst_buffer, dst_size);
    }

    delete[] heap_buffer;
    return result;
}
