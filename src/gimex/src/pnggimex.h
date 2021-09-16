/**
 * @file
 *
 * @brief Png GIMEX codec implementation.
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

int GIMEX_API PNG_is(GSTREAM *stream);
int GIMEX_API PNG_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API PNG_close(GINSTANCE *ctx);
int GIMEX_API PNG_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
int GIMEX_API PNG_wclose(GINSTANCE *ctx);
GINFO *GIMEX_API PNG_info(GINSTANCE *ctx, int frame);
int GIMEX_API PNG_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch);
int GIMEX_API PNG_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch);
GABOUT *GIMEX_API PNG_about(void);

#ifdef __cplusplus
} // extern "C"
#endif
