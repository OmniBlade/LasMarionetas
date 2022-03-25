/**
 * @file
 *
 * @brief Bitmap data structures and constants.
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
#endif

/* Pack the structs to align with on disk layout */
#pragma pack(push, 1)

#define TGA_TYPE_NONE 0
#define TGA_TYPE_MAPPED 1
#define TGA_TYPE_COLOR 2
#define TGA_TYPE_GREY 3
#define TGA_TYPE_COMPRESS_FLAG 8
#define TGA_TYPE_RLE_MAPPED (TGA_TYPE_MAPPED | TGA_TYPE_COMPRESS_FLAG)
#define TGA_TYPE_RLE_COLOR (TGA_TYPE_COLOR | TGA_TYPE_COMPRESS_FLAG)
#define TGA_TYPE_RLE_GREY (TGA_TYPE_GREY | TGA_TYPE_COMPRESS_FLAG)

#define TGA_HAS_MAP 1

typedef struct
{
    int8_t id_length;
    uint8_t cmap_type;
    uint8_t image_type;
    uint16_t cmap_start;
    uint16_t cmap_length;
    uint8_t cmap_depth;
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t width;
    uint16_t height;
    int8_t pixel_depth;
    int8_t image_descriptor;
} TGAHeader;

#pragma pack(pop)

#ifdef __cplusplus
} // extern "C"
#endif
