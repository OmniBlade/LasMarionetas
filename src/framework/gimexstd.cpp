/**
 * @file
 *
 * @brief Implements the GIMEX standard functions for GZ/RZ applications.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "igzfile.h"
#include <gimex.h>

void *GIMEX_API galloc(uint32_t size)
{
    return new char[size];
}

int GIMEX_API gfree(void *ptr)
{
    delete static_cast<char *>(ptr);
    return true;
}

uint32_t GIMEX_API gread(GSTREAM *stream, void *dst, int32_t size)
{
    if (size < 0) {
        return 0;
    }

    uint32_t count = size;
    reinterpret_cast<cIGZFile *>(stream)->ReadWithCount(dst, count);

    return count;
}

uint32_t GIMEX_API gwrite(GSTREAM *stream, void *src, int32_t size)
{
    if (size < 0) {
        return 0;
    }

    uint32_t count = size;
    reinterpret_cast<cIGZFile *>(stream)->WriteWithCount(src, count);

    return count;
}

int GIMEX_API gseek(GSTREAM *stream, int32_t pos, int32_t whence)
{
    return pos == reinterpret_cast<cIGZFile *>(stream)->SeekToPosition((uint32_t)pos);
}

int64_t GIMEX_API glen(GSTREAM *stream)
{
    return reinterpret_cast<cIGZFile *>(stream)->Length();
}
