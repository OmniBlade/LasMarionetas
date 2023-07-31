/**
 * @file
 *
 * @brief Gimex public interface implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "gfuncs.h"
#include <ctype.h>
#include <gimex.h>
#include <stddef.h>

static int gCurrentGimex;

int GIMEX_API GIMEX_is(GSTREAM *stream)
{
    return gFunctions[gCurrentGimex].is(stream);
}

int GIMEX_API GIMEX_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    return gFunctions[gCurrentGimex].open(ctx, stream, unk1, unk2);
}

int GIMEX_API GIMEX_close(GINSTANCE *ctx)
{
    return gFunctions[gCurrentGimex].close(ctx);
}

int GIMEX_API GIMEX_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    return gFunctions[gCurrentGimex].wopen(ctx, stream, unk1, unk2);
}

int GIMEX_API GIMEX_wclose(GINSTANCE *ctx)
{
    return gFunctions[gCurrentGimex].wclose(ctx);
}

GINFO *GIMEX_API GIMEX_info(GINSTANCE *ctx, int frame)
{
    return gFunctions[gCurrentGimex].info(ctx, frame);
}

bool GIMEX_API GIMEX_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    return gFunctions[gCurrentGimex].read(ctx, info, buffer, pitch) != 0;
}

bool GIMEX_API GIMEX_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    return gFunctions[gCurrentGimex].write(ctx, info, buffer, pitch) != 0;
}

GABOUT *GIMEX_API GIMEX_about(void)
{
    return gFunctions[gCurrentGimex].about();
}

int GIMEX_max()
{
    int max_codec = 0;

    while (gFunctions[max_codec].about != NULL) {
        ++max_codec;
    }

    return max_codec;
}

void GIMEX_API GIMEX_set(int codec)
{
    int max_codec = GIMEX_max();
    gCurrentGimex = codec < max_codec && codec > 0 ? codec : 0;
}

int GIMEX_API GIMEX_get(void)
{
    return gCurrentGimex;
}

int GIMEX_strcmp(const char *str1, const char *str2)
{
    int ret_val;

    do {
        int str1_val;
        int str2_val;

        if (*str2 < 'A' || *str2 > 'Z') {
            str2_val = *str2;
        } else {
            str2_val = *str2 + 32;
        }

        if (*str1 < 'A' || *str1 > 'Z') {
            str1_val = *str1;
        } else {
            str1_val = *str1 + 32;
        }

        ret_val = str2_val - str1_val;

        if (*str2 == '\0') {
            break;
        }

        ++str1;
        ++str2;
    } while (!ret_val);

    return ret_val;
}

int GIMEX_API GIMEX_find(const char *formats, int *format_index)
{
    const char *get_ptr = formats;
    char ext_buffer[1024];
    char *put_ptr = ext_buffer;
    int gimex_format = 0;
    int i;
    int j;

    if (*formats == '-' || *formats == '+' || *formats == '.') {
        ++get_ptr;
    }

    *put_ptr++ = '.';

    if (*get_ptr > ' ') {
        int current_best = 0;

        while (put_ptr <= ext_buffer + sizeof(ext_buffer)) {
            if (*get_ptr <= ' ' || *get_ptr == '?') {
                break;
            }

            *put_ptr++ = *get_ptr++;
        };
        *put_ptr = '\0';

        for (i = 0; i < GIMEX_EXTENSIONS; ++i) {
            if (current_best >= 4) {
                break;
            }

            for (j = 1; j < GIMEX_max(); ++j) {
                GABOUT *about;

                if (current_best >= 4) {
                    break;
                }

                GIMEX_set(j);
                about = GIMEX_about();

                if (about != NULL) {
                    if (!GIMEX_strcmp(about->extensions[i], ext_buffer)) {
                        int match_score = i == 0 ? 2 : 1;

                        if (about->can_export) {
                            match_score += 2;
                        }

                        if (match_score > current_best) {
                            current_best = match_score;
                            gimex_format = j;

                            if (format_index != NULL) {
                                *format_index = i;
                            }
                        }
                    }

                    gfree(about);
                }
            }
        }
    }

    GIMEX_set(gimex_format);
    return gimex_format;
}
