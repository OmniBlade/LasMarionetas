/**
 * @file
 *
 * @brief EA Shape GIMEX codec implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "shpgimex.h"
#include "shapefile.h"
#include <endianness.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int GIMEX_API FSH_is(GSTREAM *stream)
{
    SHAPEHEADERDIR header;

    gseek(stream, 0, GSEEK_SET);

    if (gread(stream, &header, sizeof(header)) == sizeof(header)) {
        uint32_t fourcc = be32toh(header.mHeader.mnFourCC);

        if (fourcc == GIMEX_ID('S', 'h', 'p', 'F')) {
            printf("This file is a ShpF type file.\n");
            return 100;
        }

        if (fourcc == GIMEX_ID('F', 'n', 't', 'F')) {
            printf("This file is a FntF type file.\n");
            return 98;
        }
    }

    return 0;
}

int GIMEX_API FSH_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API FSH_close(GINSTANCE *ctx)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API FSH_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API FSH_wclose(GINSTANCE *ctx)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

GINFO *GIMEX_API FSH_info(GINSTANCE *ctx, int frame)
{
    GIMEX_NOTIMPLEMENTED();
    GINFO *info = galloc(sizeof(GINFO));
    if (info) {
        memset(info, 0, sizeof(GINFO));
    }
    return info;
}

int GIMEX_API FSH_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API FSH_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

GABOUT *GIMEX_API FSH_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID('S', 'h', 'p', 'F');
        about->size = sizeof(GABOUT);
        about->version = GIMEX_VERSION;
        about->can_import = 1;
        about->can_export = 1;
        about->import_packed = 3;
        about->export_packed = 3;
        about->import_paletted = 1;
        about->export_paletted = 1;
        about->import_argb = 1;
        about->export_argb = 1;
        about->multi_frame = 1;
        about->multi_file = 0;
        about->multi_size = 1;
        about->requires_frame_buffer = 1;
        about->external = 0;
        about->uses_file = 1;
        about->mip_maps = 1;
        about->max_frame_name = 504;
        about->default_quality = 75;
        about->mac_type[0] = GIMEX_ID('.', 'f', 's', 'h');
        strcpy(about->extensions[0], ".fsh");
        strcpy(about->extensions[1], ".newfsh");
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "FSH");
        strcpy(about->word_type, "FSH");
        strcpy(about->long_type, "PC Shape File");
    }

    return about;
}
