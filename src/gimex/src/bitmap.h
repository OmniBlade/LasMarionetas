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

/* Constants for the biCompression field */
#define BI_RGB 0 /* No compression - straight BGR data */
#define BI_RLE8 1 /* 8-bit run-length compression */
#define BI_RLE4 2 /* 4-bit run-length compression */
#define BI_BITFIELDS 3 /* RGB bitmap with RGB masks */

/* Bitmap header magic number "MB" */
#define BF_TYPE 0x4D42

/* Bitmap types */
#define BMP_UNKNOWN 0
#define BMP_CORE 1
#define BMP_INFOEX 3
#define BMP_V4 2
#define BMP_INFO 4

/* Pack the structs to align with on disk layout */
#pragma pack(push, 1)
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
} CIEXYZ;

typedef struct
{
    CIEXYZ red;
    CIEXYZ green;
    CIEXYZ blue;
} CIEXYZTRIPLE;

typedef struct
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;
} BITMAPFILEHEADER;

typedef struct
{
    uint32_t size;
    uint16_t width;
    uint16_t height;
    uint16_t planes;
    uint16_t bit_count;
} BITMAPCOREHEADER;

typedef struct
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t xpels_per_meter;
    int32_t ypels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
} BITMAPINFOHEADER;

typedef struct
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t xpels_per_meter;
    int32_t ypels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t alpha_mask;
    uint32_t cs_type;
    CIEXYZTRIPLE end_points;
    uint32_t gamma_red;
    uint32_t gamma_green;
    uint32_t gamma_blue;
} BITMAPV4HEADER;

typedef struct {
    BITMAPFILEHEADER file;
    BITMAPV4HEADER bmp;
} BITMAPHEADER;

#pragma pack(pop)

#ifdef __cplusplus
} // extern "C"
#endif
