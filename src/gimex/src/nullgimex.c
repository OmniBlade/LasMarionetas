/**
 * @file
 *
 * @brief Null GIMEX codec implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "nullgimex.h"
#include <stddef.h>
#include <string.h>

int GIMEX_API NULL_is(GSTREAM *stream)
{
    return 0;
}

int GIMEX_API NULL_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    return 0;
}

int GIMEX_API NULL_close(GINSTANCE *ctx)
{
    return 0;
}

int GIMEX_API NULL_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    return 0;
}

int GIMEX_API NULL_wclose(GINSTANCE *ctx)
{
    return 0;
}

GINFO *GIMEX_API NULL_info(GINSTANCE *ctx, int frame)
{
    return NULL;
}

int GIMEX_API NULL_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    return 0;
}

int GIMEX_API NULL_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    return 0;
}

GABOUT *GIMEX_API NULL_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID('N', 'U', 'L', 'L');
        about->size = sizeof(GABOUT);
        about->version = GIMEX_VERSION;
        about->can_import = 0;
        about->can_export = 0;
        about->import_packed = 0;
        about->export_packed = 0;
        about->import_paletted = 0;
        about->export_paletted = 0;
        about->import_argb = 0;
        about->export_argb = 0;
        about->multi_frame = 0;
        about->multi_file = 0;
        about->multi_size = 1;
        about->requires_frame_buffer = 0;
        about->external = 0;
        about->uses_file = 0;
        about->max_frame_name = 0;
        about->default_quality = 100;
        about->mac_type[0] = 0;
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "Null");
        strcpy(about->word_type, "Not");
        strcpy(about->long_type, "Not Gimex");
    }

    return about;
}
