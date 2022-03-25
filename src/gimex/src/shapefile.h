/**
 * @file
 *
 * @brief EA Shape image format.
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

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#pragma pack(push, 1)
typedef struct _SHAPEHEADER
{
    uint32_t mnFourCC;
    int32_t mnFileSize;
    int32_t mnEntries;
    uint32_t mnID;
} SHAPEHEADER;

typedef struct _SHAPEDIRECTORY
{
    int32_t mnOffset;
    char msDesc[8];
} SHAPEDIRECTORY;

typedef struct _SHAPEHEADERDIR
{
    SHAPEHEADER mHeader;
    SHAPEDIRECTORY mDirectory[2];
} SHAPEHEADERDIR;

typedef struct _ShapeList
{
    int unk;
} ShapeList;
#pragma pack(pop)

int SHPRReadShapeFile(
    void *stream, ShapeList **list, int unk1, int unk2, unsigned pimary_id, unsigned secondary_id, int unk3);

#ifdef __cplusplus
} // extern "C"
#endif
