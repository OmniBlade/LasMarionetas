/**
 * @file
 *
 * @brief JPEG GIMEX codec implementation.
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

int GIMEX_API JPG_is(GSTREAM *stream);
int GIMEX_API JPG_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API JPG_close(GINSTANCE *ctx);
int GIMEX_API JPG_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API JPG_wclose(GINSTANCE *ctx);
GINFO *GIMEX_API JPG_info(GINSTANCE *ctx, int frame);
int GIMEX_API JPG_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch);
int GIMEX_API JPG_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch);
GABOUT *GIMEX_API JPG_about(void);

int GIMEX_API IJL_is(GSTREAM *stream);
int GIMEX_API IJL_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API IJL_close(GINSTANCE *ctx);
int GIMEX_API IJL_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API IJL_wclose(GINSTANCE *ctx);
GINFO *GIMEX_API IJL_info(GINSTANCE *ctx, int frame);
int GIMEX_API IJL_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch);
int GIMEX_API IJL_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch);
GABOUT *GIMEX_API IJL_about(void);

#ifdef __cplusplus
} // extern "C"
#endif
