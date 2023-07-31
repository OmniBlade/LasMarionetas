/**
 * @file
 *
 * @brief Targa GIMEX codec implementation.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "targagimex.h"
#include "targa.h"
#include <endianness.h>
#include <stddef.h>
#include <string.h>

static void TGA_flipline(uint8_t *dst, int width, int bpp)
{
    if (bpp == 32) {
        uint32_t *start = (uint32_t *)(dst);
        uint32_t *end = ((uint32_t *)dst) + width - 1;

        for (int i = 0; i < (width / 2); ++i) {
            uint32_t tmp = *start;
            *start++ = *end;
            *end-- = tmp;
        }
    } else {
        uint8_t *start = dst;
        uint8_t *end = dst + width - 1;

        for (int i = 0; i < (width / 2); ++i) {
            uint8_t swapchar = *start;
            *start++ = *end;
            *end-- = swapchar;
        }
    }
}

static int TGA_readline(GINFO *info, uint8_t *dst, uint8_t *buffer, GSTREAM *stream)
{
    uint8_t *getp = buffer;
    uint8_t *putp = dst;
    int retval = 0;
    int32_t width = info->width;
    int32_t bpp = info->original_bpp;

    if (info->packed) {
        int x = 0;
        int count = 0;

        do {
            retval = gread(stream, getp, 1);
            count = *getp;
            getp = buffer;

            /* Handle run of repeats */
            if (count & 0x80) {
                count = (count & 0x7F) + 1;
                x += count;

                switch (bpp) {
                    case 8:
                        retval = gread(stream, getp, 1);

                        for (int i = 0; i < count; ++i) {
                            *putp++ = *getp;
                        }

                        break;

                    case 15:
                    case 16:
                        retval = gread(stream, getp, 2);

                        for (int i = 0; i < count; ++i) {
                            if (info->alpha_bits != 0) {
                                ((ARGB *)putp)->a = 255 * ((*(getp + 1) >> 7) & 1);
                            } else {
                                ((ARGB *)putp)->a = 0xFF;
                            }

                            ((ARGB *)putp)->r = 255 * ((*(getp + 1) >> 2) & 31) / 31;
                            ((ARGB *)putp)->g = 255 * (((*(getp + 1) & 3) << 3) | ((*(getp + 0) >> 5) & 7)) / 31;
                            ((ARGB *)putp)->b = 255 * (*getp & 31) / 31;
                            putp += 4;
                        }

                        break;

                    case 24:
                        retval = gread(stream, getp, 3);

                        for (int i = 0; i < count; ++i) {
                            ((ARGB *)putp)->a = 0xFF;
                            ((ARGB *)putp)->r = getp[2];
                            ((ARGB *)putp)->g = getp[1];
                            ((ARGB *)putp)->b = getp[0];
                            putp += 4;
                        }

                        break;

                    case 32:
                        retval = gread(stream, getp, 4);

                        for (int i = 0; i < count; ++i) {
                            ((ARGB *)putp)->a = getp[3];
                            ((ARGB *)putp)->r = getp[2];
                            ((ARGB *)putp)->g = getp[1];
                            ((ARGB *)putp)->b = getp[0];
                            putp += 4;
                        }

                        break;
                }
            } else {
                count = (count & 0x7F) + 1;
                x += count;
                retval = gread(stream, getp, (count * ((bpp + 7) & ~7)) >> 3);

                switch (bpp) {
                    case 8:
                        for (int i = 0; i < count; ++i) {
                            *putp++ = *getp++;
                        }

                        break;

                    case 15:
                    case 16:
                        for (int i = 0; i < count; ++i) {
                            if (info->alpha_bits != 0) {
                                ((ARGB *)putp)->a = 255 * ((*(getp + 1) >> 7) & 1);
                            } else {
                                ((ARGB *)putp)->a = 0xFF;
                            }

                            ((ARGB *)putp)->r = 255 * ((*(getp + 1) >> 2) & 31) / 31;
                            ((ARGB *)putp)->g = 255 * (((*(getp + 1) & 3) << 3) | ((*(getp + 0) >> 5) & 7)) / 31;
                            ((ARGB *)putp)->b = 255 * (*getp & 31) / 31;
                            putp += 4;
                            getp += 2;
                        }

                        break;

                    case 24:
                        for (int i = 0; i < count; ++i) {
                            ((ARGB *)putp)->a = 0xFF;
                            ((ARGB *)putp)->r = getp[2];
                            ((ARGB *)putp)->g = getp[1];
                            ((ARGB *)putp)->b = getp[0];
                            putp += 4;
                            getp += 3;
                        }

                        break;

                    case 32:
                        for (int i = 0; i < count; ++i) {
                            ((ARGB *)putp)->a = getp[3];
                            ((ARGB *)putp)->r = getp[2];
                            ((ARGB *)putp)->g = getp[1];
                            ((ARGB *)putp)->b = getp[0];
                            putp += 4;
                            getp += 4;
                        }

                        break;
                }
            }
        } while (x < width);
    } else {
        retval = gread(stream, getp, (width * ((bpp + 7) & ~7)) >> 3);

        switch (bpp) {
            case 8:
                for (int i = 0; i < width; ++i) {
                    *putp++ = *getp++;
                }

                break;

            case 15:
            case 16:
                for (int i = 0; i < width; ++i) {
                    if (info->alpha_bits != 0) {
                        ((ARGB *)putp)->a = 255 * ((*(getp + 1) >> 7) & 1);
                    } else {
                        ((ARGB *)putp)->a = 0xFF;
                    }

                    ((ARGB *)putp)->r = 255 * ((*(getp + 1) >> 2) & 31) / 31;
                    ((ARGB *)putp)->g = 255 * (((*(getp + 1) & 3) << 3) | ((*(getp + 0) >> 5) & 7)) / 31;
                    ((ARGB *)putp)->b = 255 * (*getp & 31) / 31;
                    putp += 4;
                    getp += 2;
                }

                break;

            case 24:
                for (int i = 0; i < width; ++i) {
                    ((ARGB *)putp)->a = 0xFF;
                    ((ARGB *)putp)->r = getp[2];
                    ((ARGB *)putp)->g = getp[1];
                    ((ARGB *)putp)->b = getp[0];
                    putp += 4;
                    getp += 3;
                }

                break;

            case 32:
                for (int i = 0; i < width; ++i) {
                    ((ARGB *)putp)->a = getp[3];
                    ((ARGB *)putp)->r = getp[2];
                    ((ARGB *)putp)->g = getp[1];
                    ((ARGB *)putp)->b = getp[0];
                    putp += 4;
                    getp += 4;
                }

                break;
        }
    }

    return retval;
}

static uint32_t TGA_pixelval(uint8_t *src, int size)
{
    switch (size) {
        case 1:
            return *src;
        case 2:
            return le16toh(*(uint16_t *)src);
        case 3:
            return (src[2] << 16) | (src[1] << 8) | src[0];
        case 4:
            return le32toh(*(uint32_t *)src);
        default:
            break;
    }

    return 0;
}

static uint8_t TGA_rlecount(uint8_t *src, int max_count, int pixel_size)
{
    uint8_t count = 0;

    if (max_count > 128) {
        max_count = 128;
    }

    while (count < max_count) {
        if (TGA_pixelval(src, pixel_size) != TGA_pixelval(src + count * pixel_size, pixel_size)) {
            break;
        }

        ++count;
    }

    return count;
}

static int TGA_writeline(
    uint8_t *src, uint8_t *dst, int width, int bpp, int src_bpp, uint8_t *grey_pal, int compress, int alpha_bits)
{
    uint8_t *getp = src;
    uint8_t *putp = dst;

    if (bpp == 15) {
        bpp = 16;
    }

    if (compress) {
        uint8_t count_one;
        uint8_t count_two;
        int pixel_size = bpp == 8 ? 1 : 4;

        while (width > 0) {
            count_one = TGA_rlecount(getp, width, pixel_size);
            count_two = TGA_rlecount(getp + count_one * pixel_size, width - count_one, pixel_size);

            if (count_one >= 3 || (count_one >= 2 && (count_two >= 2 || bpp > 8))) {
                *putp++ = 0x80 | count_one - 1;

                if (bpp == 32) {
                    putp[0] = ((ARGB *)getp)->b;
                    putp[1] = ((ARGB *)getp)->g;
                    putp[2] = ((ARGB *)getp)->r;
                    putp[3] = ((ARGB *)getp)->a;
                    putp += 4;
                    getp += 4 * count_one;
                } else if (bpp == 24) {
                    putp[0] = ((ARGB *)getp)->b;
                    putp[1] = ((ARGB *)getp)->g;
                    putp[2] = ((ARGB *)getp)->r;
                    putp += 3;
                    getp += 4 * count_one;
                } else if (bpp == 16) {
                    uint32_t a = 255;
                    uint32_t r;
                    uint32_t g;
                    uint32_t b;
                    uint32_t color;

                    if (alpha_bits) {
                        a = ((ARGB *)getp)->a >> 7;
                    }

                    r = ((ARGB *)getp)->r >> 3;
                    g = ((ARGB *)getp)->g >> 3;
                    b = ((ARGB *)getp)->b >> 3;
                    color = (a << 15) | (r << 10) | (g << 5) | b;
                    *(uint16_t *)putp = htole16(color);
                    putp += 2;
                    getp += 4 * count_one;
                } else if (bpp == 8) {
                    if (src_bpp == 8) {
                        *putp++ = grey_pal[*getp];
                        getp += count_one;
                    } else {
                        *putp++ = ((ARGB *)getp)->r;
                        getp += 4 * count_one;
                    }
                }

                width -= count_one;
            } else {
                for (count_one = 1; count_one < width && count_one < 128; ++count_one) {
                    count_two = TGA_rlecount(getp + count_one * pixel_size, width - count_one, pixel_size);

                    if (count_two >= 3 || (count_two >= 2 && bpp > 8)) {
                        break;
                    }
                }
                if (count_one >= (width - 3))
                    count_one = width;
                if (count_one > 128)
                    count_one = 128;

                *putp++ = (unsigned char)(count_one - 1);

                while (count_one--) {
                    --width;
                    if (bpp == 32) {
                        putp[3] = ((ARGB *)getp)->a;
                        putp[2] = ((ARGB *)getp)->r;
                        putp[1] = ((ARGB *)getp)->g;
                        putp[0] = ((ARGB *)getp)->b;
                        putp += 4;
                        getp += 4;
                    } else if (bpp == 24) {
                        putp[2] = ((ARGB *)getp)->r;
                        putp[1] = ((ARGB *)getp)->g;
                        putp[0] = ((ARGB *)getp)->b;
                        putp += 3;
                        getp += 4;
                    } else if (bpp == 16) {
                        uint32_t a = 255;
                        uint32_t r;
                        uint32_t g;
                        uint32_t b;
                        uint32_t color;

                        if (alpha_bits) {
                            a = ((ARGB *)getp)->a >> 7;
                        }

                        r = ((ARGB *)getp)->r >> 3;
                        g = ((ARGB *)getp)->g >> 3;
                        b = ((ARGB *)getp)->b >> 3;
                        color = (a << 15) | (r << 10) | (g << 5) | b;
                        *(uint16_t *)putp = htole16(color);
                        putp += 2;
                        getp += 4;
                    } else if (bpp == 8) {
                        if (src_bpp == 8) {
                            *putp++ = grey_pal[*getp];
                            getp += 1;
                        } else {
                            *putp++ = ((ARGB *)getp)->r;
                            getp += 4;
                        }
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < width; ++i) {
            if (bpp == 32) {
                putp[0] = ((ARGB *)getp)->b;
                putp[1] = ((ARGB *)getp)->g;
                putp[2] = ((ARGB *)getp)->r;
                putp[3] = ((ARGB *)getp)->a;
                putp += 4;
                getp += 4;
            } else if (bpp == 24) {
                putp[0] = ((ARGB *)getp)->b;
                putp[1] = ((ARGB *)getp)->g;
                putp[2] = ((ARGB *)getp)->r;
                putp += 3;
                getp += 4;
            } else if (bpp == 16) {
                uint32_t a = 255;
                uint32_t r;
                uint32_t g;
                uint32_t b;
                uint32_t color;

                if (alpha_bits) {
                    a = ((ARGB *)getp)->a >> 7;
                }

                r = ((ARGB *)getp)->r >> 3;
                g = ((ARGB *)getp)->g >> 3;
                b = ((ARGB *)getp)->b >> 3;
                color = (a << 15) | (r << 10) | (g << 5) | b;
                *(uint16_t *)putp = htole16(color);
                putp += 2;
                getp += 4;
            } else if (bpp == 8) {
                if (src_bpp == 8)
                    *putp++ = grey_pal[*getp++];
                else {
                    *putp++ = ((ARGB *)getp)->r;
                    getp += 4;
                }
            }
        }
    }

    return putp - dst;
}

int GIMEX_API TGA_is(GSTREAM *stream)
{
    TGAHeader header;

    gseek(stream, 0, GSEEK_SET);

    if (!gread(stream, &header, sizeof(header))) {
        return 0;
    }

    if (header.cmap_type <= 1
        && (header.image_type == TGA_TYPE_MAPPED || header.image_type == TGA_TYPE_COLOR || header.image_type == TGA_TYPE_GREY
            || header.image_type == TGA_TYPE_RLE_MAPPED || header.image_type == TGA_TYPE_RLE_COLOR
            || header.image_type == TGA_TYPE_RLE_GREY)
        && (header.pixel_depth == 8 || header.pixel_depth == 15 || header.pixel_depth == 16 || header.pixel_depth == 24
            || header.pixel_depth == 32)) {

        if (le16toh(header.width) <= 4096 && le16toh(header.height) != 0 && le16toh(header.height) <= 2048
            && le16toh(header.cmap_start) <= 255 && le16toh(header.cmap_length) <= 256 && header.cmap_depth <= 32
            && le16toh(header.x_offset) <= 4096 && le16toh(header.y_offset) <= 2048) {
            return 100;
        } else {
            return 60;
        }
    }

    return 0;
}

int GIMEX_API TGA_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst = galloc(sizeof(GINSTANCE));

    if (!inst) {
        return 0;
    }

    memset(inst, 0, sizeof(GINSTANCE));
    inst->frames = 1;
    inst->signature = GIMEX_ID('.', 'T', 'G', 'A');
    inst->size = sizeof(GINSTANCE);
    inst->stream = stream;
    *ctx = inst;

    return 1;
}

int GIMEX_API TGA_close(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

int GIMEX_API TGA_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst = galloc(sizeof(GINSTANCE));

    if (inst == NULL) {
        return 0;
    }

    memset(inst, 0, sizeof(GINSTANCE));
    inst->stream = stream;
    inst->frame_num = 0;
    *ctx = inst;

    return 1;
}

int GIMEX_API TGA_wclose(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

GINFO *GIMEX_API TGA_info(GINSTANCE *ctx, int frame)
{
    TGAHeader header;
    GINFO *info = NULL;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    int32_t type = TGA_TYPE_NONE;
    int32_t compressed = 0;
    int32_t cmap_start = 0;
    int32_t cmap_length = 0;
    int32_t cmap_bpp = 0;
    int32_t x_offset = 0;
    int32_t y_offset = 0;
    int32_t alpha_bits = 0;
    uint32_t colors = 0;
    uint32_t image_size;

    gseek(ctx->stream, 0, GSEEK_SET);
    image_size = (uint32_t)glen(ctx->stream);

    if (!gread(ctx->stream, &header, sizeof(header))) {
        return NULL;
    }

    width = le16toh(header.width);
    height = le16toh(header.height);
    bpp = header.pixel_depth;
    type = header.image_type & ~TGA_TYPE_COMPRESS_FLAG;
    compressed = (header.image_type & TGA_TYPE_COMPRESS_FLAG) != 0;
    cmap_start = le16toh(header.cmap_start);
    cmap_length = le16toh(header.cmap_length);
    cmap_bpp = header.cmap_depth;
    x_offset = le16toh(header.x_offset);
    y_offset = le16toh(header.y_offset);

    if ((header.image_descriptor & 0x20) == 0) {
        y_offset = -y_offset;
    }
    if ((header.image_descriptor & 0x10) != 0) {
        x_offset = -x_offset;
    }

    if (header.pixel_depth == 15 || header.pixel_depth == 16
        || (header.pixel_depth == 8 && header.cmap_type && (header.cmap_depth == 15 || header.cmap_depth == 16))) {
        alpha_bits = (header.image_descriptor & 0xF) == 1;
    } else {
        alpha_bits = bpp - 24;
    }

    if (alpha_bits <= 0) {
        alpha_bits = 0;
    }

    if (alpha_bits > 8) {
        alpha_bits = 8;
    }

    gseek(ctx->stream, header.id_length + sizeof(header), GSEEK_SET);

    if (type == TGA_TYPE_MAPPED) {
        colors = cmap_length;
    } else if (type == TGA_TYPE_GREY) {
        colors = 256;
    }

    info = (GINFO *)galloc(sizeof(GINFO));

    if (info) {
        memset(info, 0, sizeof(GINFO));
        info->signature = GIMEX_ID('.', 'T', 'G', 'A');
        info->size = sizeof(GINFO);
        info->version = GIMEX_VERSION;
        info->frame_num = 0;
        info->width = width;
        info->height = height;
        info->frame_size = image_size;
        info->sub_type = TGA_TYPE_NONE;
        info->packed = compressed;
        info->quality = 100;
        info->alpha_bits = alpha_bits;
        info->red_bits = 8;
        info->green_bits = 8;
        info->blue_bits = 8;
        info->center_x = 0;
        info->center_y = 0;
        info->default_x = x_offset;
        info->default_y = y_offset;
        info->num_hotspots = 0;
        info->frame_name[0] = '\0';

        if (bpp < 15) {
            if (bpp == 0) {
                bpp = 8;
            }

            if (colors == 0) {
                colors = 1 << bpp;
            }

            if (colors > 256) {
                colors = 256;
            }

            info->bpp = 8;
            info->original_bpp = bpp;
            info->num_colors = colors;
            info->palette_start_index = cmap_start;

            if (header.cmap_type != TGA_HAS_MAP) { /* Grey scale or paletted */
                info->alpha_bits = 0;

                for (int i = 0; i < cmap_length; ++i) {
                    info->colortbl[cmap_start + i].a = 0xFF;
                    info->colortbl[cmap_start + i].r = i;
                    info->colortbl[cmap_start + i].g = i;
                    info->colortbl[cmap_start + i].b = i;
                }
            } else {
                uint8_t pal[256 * 4];

                switch (cmap_bpp) {
                    case 15:
                    case 16: /* Fallthrough */
                        info->alpha_bits = 0;
                        info->red_bits = 5;
                        info->green_bits = 5;
                        info->blue_bits = 5;

                        if (!gread(ctx->stream, (char *)pal, colors * 2)) {
                            gfree(info);
                            return NULL;
                        }

                        for (int i = 0; i < cmap_length; ++i) {
                            uint16_t pixel = le16toh(((uint16_t *)pal)[i]);

                            if (alpha_bits) {
                                info->colortbl[cmap_start + i].a = -((pixel & 0x8000) != 0);
                            } else {
                                info->colortbl[cmap_start + i].a = 0xFF;
                            }

                            info->colortbl[cmap_start + i].r = 255 * ((pixel >> 10) & 31) / 31;
                            info->colortbl[cmap_start + i].g = 255 * ((pixel >> 5) & 31) / 31;
                            info->colortbl[cmap_start + i].b = 255 * ((pixel >> 0) & 31) / 31;
                        }
                        break;
                    case 24:
                        info->alpha_bits = 0;

                        if (!gread(ctx->stream, (char *)pal, colors * 3)) {
                            gfree(info);
                            return NULL;
                        }

                        for (int i = 0; i < cmap_length; ++i) {
                            info->colortbl[cmap_start + i].a = 0xFF;
                            info->colortbl[cmap_start + i].r = pal[i * 3 + 2];
                            info->colortbl[cmap_start + i].g = pal[i * 3 + 1];
                            info->colortbl[cmap_start + i].b = pal[i * 3 + 0];
                        }
                        break;
                    case 32:
                        info->alpha_bits = 8;

                        if (!gread(ctx->stream, (char *)pal, colors * 4)) {
                            gfree(info);
                            return NULL;
                        }

                        for (int i = 0; i < cmap_length; ++i) {
                            info->colortbl[cmap_start + i].a = pal[i * 4 + 3];
                            info->colortbl[cmap_start + i].r = pal[i * 4 + 2];
                            info->colortbl[cmap_start + i].g = pal[i * 4 + 1];
                            info->colortbl[cmap_start + i].b = pal[i * 4 + 0];
                        }
                        break;

                    default:
                        break;
                }
            }
        } else {
            if (bpp == 16) {
                bpp = 15;
            }

            info->num_colors = 0;
            info->bpp = 32;
            info->original_bpp = bpp;

            if (bpp <= 16) {
                info->blue_bits = 5;
                info->green_bits = 5;
                info->red_bits = 5;
            }

            if (bpp < 32) {
                info->alpha_bits = 0;
            }
        }
    }

    return info;
}

int GIMEX_API TGA_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    TGAHeader header;
    char *putp = NULL;
    uint8_t *line_buffer = NULL;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    int32_t offset = 0;
    int32_t actual_bpp = 0;
    int retval = 0;

    gseek(ctx->stream, 0, GSEEK_SET);

    if (!gread(ctx->stream, &header, sizeof(header))) {
        return 0;
    }

    bpp = info->original_bpp;
    width = info->width;
    height = info->height;
    offset = (uint8_t)header.id_length + 18;

    if (header.cmap_type >= TGA_HAS_MAP) {
        offset += le32toh(header.cmap_length) * ((header.cmap_depth + 7) / 8);
    }

    retval = gseek(ctx->stream, offset, GSEEK_SET);

    /* Handle the row order of the image data */
    if (header.image_descriptor & 0x20) {
        putp = buffer;
    } else {
        putp = &buffer[pitch * (height - 1)];
        pitch = -pitch;
    }

    actual_bpp = bpp != 15 ? bpp : 16;
    line_buffer = galloc(((width * actual_bpp + 7) & ~7) >> 3);

    if (line_buffer == NULL) {
        return 0;
    }

    for (int i = 0; i < height && retval != 0; ++i) {
        if (i == 123) {
            offset = 0;
        }
        retval = TGA_readline(info, (uint8_t *)putp, line_buffer, ctx->stream);

        /* Handle column order of the image data */
        if (header.image_descriptor & 0x10) {
            TGA_flipline((uint8_t *)putp, width, bpp);
        }

        putp += pitch;
    }

    gfree(line_buffer);

    return retval;
}

int GIMEX_API TGA_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    TGAHeader header;
    uint8_t pal_buff[256 * 4];
    uint8_t grey_buff[256];
    uint8_t *line_buffer = NULL;
    int retval = 0;
    int32_t header_size = 0;
    int32_t image_size = 0;
    int32_t bpp = 0;
    int32_t src_pitch = 0;
    int32_t descriptor = info->alpha_bits;
    int32_t pal_type = 0;
    int32_t pal_bits = 0;
    uint8_t type = TGA_TYPE_NONE;
    bool grey_scale = false;

    if (info->original_bpp <= 8) {
        bpp = 8;
    } else if (info->alpha_bits > 1 || info->red_bits > 5 || info->green_bits > 5 || info->blue_bits > 5) {
        if (info->alpha_bits != 0) {
            bpp = 32;
        } else {
            descriptor = 0;
            bpp = 24;
        }
    } else {
        bpp = 16;
    }

    src_pitch = info->width * bpp / 8;
    header_size = sizeof(header) + 4 * info->num_colors;
    image_size = src_pitch * info->height;
    type = (bpp <= 8) ? TGA_TYPE_MAPPED : TGA_TYPE_COLOR;
    pal_type = info->num_colors != 0;
    pal_bits = 32;

    if (bpp <= 8) {
        grey_scale = true;

        for (int i = 0; i < info->num_colors; ++i) {
            if (info->colortbl[i].a != 0xFF || info->colortbl[i].r != info->colortbl[i].g
                || info->colortbl[i].r != info->colortbl[i].b) {
                grey_scale = false;
            }

            grey_buff[i] = info->colortbl[i].r;
        }

        if (!grey_scale) {
            for (int i = 0; i < info->num_colors; ++i) {
                grey_buff[i] = i;
            }
        }
    }

    if (grey_scale) {
        type = TGA_TYPE_GREY;
        pal_type = 0;
    }

    if (info->packed) {
        type |= TGA_TYPE_COMPRESS_FLAG;
    }

    if (pal_type != 0) {
        if (info->red_bits != 5 || info->green_bits != 5 || info->blue_bits != 5 || info->alpha_bits > 1) {
            if (info->alpha_bits == 8) {
                pal_bits = 32;

                for (int i = 0; i < info->num_colors; ++i) {
                    pal_buff[i * 4 + 0] = info->colortbl[info->palette_start_index + i].b;
                    pal_buff[i * 4 + 1] = info->colortbl[info->palette_start_index + i].g;
                    pal_buff[i * 4 + 2] = info->colortbl[info->palette_start_index + i].r;
                    pal_buff[i * 4 + 3] = info->colortbl[info->palette_start_index + i].a;
                }
            } else {
                pal_bits = 24;

                for (int i = 0; i < info->num_colors; ++i) {
                    pal_buff[i * 3 + 0] = info->colortbl[info->palette_start_index + i].b;
                    pal_buff[i * 3 + 1] = info->colortbl[info->palette_start_index + i].g;
                    pal_buff[i * 3 + 2] = info->colortbl[info->palette_start_index + i].r;
                }
            }
        } else {
            pal_bits = 16;

            for (int i = 0; i < info->num_colors; ++i) {
                uint32_t a;
                uint32_t r;
                uint32_t g;
                uint32_t b;
                uint16_t color;

                if (descriptor != 0) {
                    a = info->colortbl[info->palette_start_index + i].r >> 7;
                } else {
                    a = 255;
                }

                r = info->colortbl[info->palette_start_index + i].r >> 3;
                g = info->colortbl[info->palette_start_index + i].g >> 3;
                b = info->colortbl[info->palette_start_index + i].b >> 3;
                color = (a << 15) | (r << 10) | (g << 5) | b;
                *(uint16_t *)&pal_buff[i * 2] = htole16(color);
            }
        }
    }

    header.image_type = type;
    header.cmap_start = htole16(info->palette_start_index);
    header.cmap_depth = pal_bits;
    header.x_offset = htole16(info->default_x);
    header.y_offset = -htole16(info->default_y);
    header.width = htole16(info->width);
    header.height = htole16(info->height);
    header.pixel_depth = bpp;
    header.image_descriptor = descriptor;
    header.id_length = 0;
    header.cmap_type = pal_type;
    header.cmap_length = htole16(info->num_colors);
    retval = gwrite(ctx->stream, &header, sizeof(header));

    if (retval != 0 && !grey_scale) {
        int pal_size = info->num_colors * pal_bits / 8;

        if (pal_size > 0) {
            retval = gwrite(ctx->stream, pal_buff, pal_size);
        }
    }

    line_buffer = galloc(src_pitch * 3 + 16);

    if (line_buffer == NULL) {
        return 0;
    }

    if (info->height > 0) {
        char *getp = (buffer + pitch * (info->height - 1));
        for (int i = 0; i < info->height; ++i) {
            int size = TGA_writeline(
                (uint8_t *)getp, line_buffer, info->width, bpp, info->bpp, grey_buff, info->packed, descriptor);
            retval = gwrite(ctx->stream, line_buffer, size);
            getp -= pitch;
        }
    }

    gfree(line_buffer);

    return 0;
}

GABOUT *GIMEX_API TGA_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID('.', 'T', 'G', 'A');
        about->size = sizeof(GABOUT);
        about->version = GIMEX_VERSION;
        about->can_import = 1;
        about->can_export = 1;
        about->import_packed = 1;
        about->export_packed = 1;
        about->import_paletted = 1;
        about->export_paletted = 1;
        about->import_argb = 1;
        about->export_argb = 1;
        about->multi_frame = 0;
        about->multi_file = 0;
        about->multi_size = 1;
        about->requires_frame_buffer = 0;
        about->external = 0;
        about->uses_file = 1;
        about->max_frame_name = 0;
        about->default_quality = 100;
        about->mac_type[0] = GIMEX_ID('T', 'P', 'I', 'C');
        strcpy(about->extensions[0], ".tga");
        strcpy(about->extensions[1], ".vda");
        strcpy(about->extensions[2], ".icb");
        strcpy(about->extensions[3], ".vst");
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "TGA");
        strcpy(about->word_type, "Targa");
        strcpy(about->long_type, "Targa");
    }

    return about;
}
