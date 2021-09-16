/**
 * @file
 *
 * @brief Bitmap GIMEX codec implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <gimex.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#define GIMEX_BMP_UNK 0
#define GIMEX_BMP_CORE 1
#define GIMEX_BMP_INFO 2
#define GIMEX_BMP_V4 3

int GIMEX_API BMP_is(GSTREAM *stream);
int GIMEX_API BMP_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API BMP_close(GINSTANCE *ctx);
int GIMEX_API BMP_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API BMP_wclose(GINSTANCE *ctx);
GINFO *GIMEX_API BMP_info(GINSTANCE *ctx, int frame);
int GIMEX_API BMP_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch);
int GIMEX_API BMP_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch);
GABOUT *GIMEX_API BMP_about(void);

#ifdef __cplusplus
} // extern "C"
#endif
