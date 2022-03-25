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
#include "bitmapgimex.h"
#include "bitmap.h"
#include <endianness.h>
#include <stddef.h>
#include <string.h>

static unsigned BMP_getbits(uint32_t mask)
{
    unsigned count = 0;

    if (mask) {
        while ((mask & 1) == 0) {
            mask >>= 1;
        }

        while ((mask & 1) != 0) {
            mask >>= 1;
            count++;
        }
    }

    return count;
}

static unsigned BMP_maskoffset(uint32_t mask)
{
    unsigned count = 0;

    if (mask) {
        while ((mask & 1) == 0) {
            mask >>= 1;
            count++;
        }
    }

    return count;
}

static int BMP_readline(GSTREAM *stream,
    uint8_t *dst,
    uint8_t *buffer,
    int bpp,
    GINFO *info,
    uint32_t a_mask,
    uint32_t r_mask,
    uint32_t g_mask,
    uint32_t b_mask)
{
    int bytes_read = 0;
    uint8_t *getp = buffer;
    int actual_bpp = bpp != 15 ? bpp : 16;
    int retval = 0;
    int run_count = 0;
    int pitch;

    if (info->packed != 0) {
        int xpos = 0;

        do {
            gread(stream, getp, 1);
            ++bytes_read;
            run_count = *getp;
            getp = buffer;

            if (run_count != 0) {
                xpos += run_count;
                gread(stream, getp, 1);
                ++bytes_read;

                switch (bpp) {
                    case 1:
                        for (int i = 0; i < run_count; ++i) {
                            *dst++ = (*getp >> (7 - (i & 7))) & 1;
                        }
                        break;
                    case 4:
                        for (int i = 0; i < run_count; ++i) {
                            if ((i & 1) != 0) {
                                *dst++ = *getp & 0xF;
                            } else {
                                *dst++ = *getp >> 4;
                            }
                        }
                        break;
                    case 8:
                        for (int i = 0; i < run_count; ++i) {
                            *dst++ = *getp;
                        }
                        break;
                    case 24:
                        gread(stream, getp + 1, 2);
                        bytes_read += 2;
                        for (int i = 0; i < run_count; ++i) {
                            ((ARGB *)dst)->a = 255;
                            ((ARGB *)dst)->r = getp[2];
                            ((ARGB *)dst)->g = getp[1];
                            ((ARGB *)dst)->b = getp[0];
                        }
                        break;
                }
            } else {
                retval = gread(stream, getp, 1);
                ++bytes_read;
                run_count = *getp;

                if (run_count >= 3) {
                    xpos += run_count;
                    pitch = ((bpp * run_count + 15) >> 3) & ~1;
                    bytes_read += pitch;
                    retval = gread(stream, getp, pitch);

                    switch (bpp) {
                        case 1:
                            for (int i = 0; i < run_count; ++i) {
                                *dst++ = (*getp++ >> (7 - (i & 7))) & 1;
                            }
                            break;
                        case 4:
                            for (int i = 0; i < run_count; ++i) {
                                if (i & 1) {
                                    *dst++ = *getp++ & 0xF;
                                } else {
                                    *dst++ = *getp >> 4;
                                }
                            }
                            break;
                        case 8:
                            for (int i = 0; i < run_count; ++i) {
                                *dst++ = *getp++;
                            }
                            break;
                        case 24:
                            for (int i = 0; i < run_count; ++i) {
                                ((ARGB *)dst)->a = 255;
                                ((ARGB *)dst)->r = getp[2];
                                ((ARGB *)dst)->g = getp[1];
                                ((ARGB *)dst)->b = getp[0];
                                dst += 4;
                                getp += 3;
                            }
                            break;
                    }
                } else if (xpos != 0) {
                    break;
                }
            }
        } while (xpos < info->width);
    } else {
        run_count = info->width;
        pitch = ((run_count * actual_bpp + 31) >> 3) & ~3;
        bytes_read += pitch;
        retval = gread(stream, getp, pitch);

        switch (bpp) {
            case 1:
                for (int i = 0; i < run_count; ++i) {
                    *dst++ = (*getp >> (7 - (i & 7))) & 1;

                    if ((i & 7) == 7) {
                        ++getp;
                    }
                }
                break;
            case 4:
                for (int i = 0; i < run_count; ++i) {
                    if (i & 1) {
                        *dst++ = *getp++ & 0xF;
                    } else {
                        *dst++ = *getp >> 4;
                    }
                }
                break;
            case 8:
                for (int i = 0; i < run_count; ++i) {
                    *dst++ = *getp++;
                }
                break;
            case 24:
                for (int i = 0; i < run_count; ++i) {
                    ((ARGB *)dst)->a = 255;
                    ((ARGB *)dst)->r = getp[2];
                    ((ARGB *)dst)->g = getp[1];
                    ((ARGB *)dst)->b = getp[0];
                    dst += 4;
                    getp += 3;
                }
                break;
            case 15:
            case 16:
            case 32: {
                int aoffset;
                int roffset;
                int goffset;
                int boffset;
                int acolors = 1 << info->alpha_bits;
                int rcolors = 1 << info->red_bits;
                int gcolors = 1 << info->green_bits;
                int bcolors = 1 << info->blue_bits;

                aoffset = BMP_maskoffset(a_mask);
                roffset = BMP_maskoffset(r_mask);
                goffset = BMP_maskoffset(g_mask);
                boffset = BMP_maskoffset(b_mask);

                for (int i = 0; i < run_count; ++i) {
                    uint32_t pixel;

                    if (bpp != 32) {
                        pixel = le16toh(*(uint16_t *)getp);
                    } else {
                        pixel = le32toh(*(uint32_t *)getp);
                    }

                    if (a_mask) {
                        unsigned temp = (a_mask & pixel) >> aoffset;
                        if (acolors != 1) {
                            temp = ((acolors >> 1) + 255 * temp) / (acolors - 1);
                        } else {
                            temp = (-(char)temp) & 0xFF;
                        }
                        ((ARGB *)dst)->a = temp;
                    } else {
                        ((ARGB *)dst)->a = 255;
                    }

                    ((ARGB *)dst)->r = (uint8_t)((((pixel & r_mask) >> roffset) * 255 + (rcolors >> 1)) / (rcolors - 1));
                    ((ARGB *)dst)->g = (uint8_t)((((pixel & g_mask) >> goffset) * 255 + (gcolors >> 1)) / (gcolors - 1));
                    ((ARGB *)dst)->b = (uint8_t)((((pixel & b_mask) >> boffset) * 255 + (bcolors >> 1)) / (bcolors - 1));
                    dst += 4;
                    getp += bpp == 16 ? 2 : 4;
                }
                break;
            }
        }
    }

    if (bytes_read & 3) {
        gread(stream, getp, -bytes_read & 3);
    }

    return retval;
}

static int BMP_runcount(uint8_t *src, int32_t max_run)
{
    int run = 0;

    if (max_run > 255) {
        max_run = 255;
    }

    while (run < max_run && src[0] == src[run]) {
        ++run;
    }

    return run;
}

static int BMP_writeline(uint8_t *src,
    uint8_t *dst,
    int32_t width,
    int32_t bpp,
    int32_t type,
    int32_t compress,
    int32_t a_bits,
    int32_t r_bits,
    int32_t g_bits,
    int32_t b_bits)
{
    uint8_t *getp = src;
    uint8_t *putp = dst;

    if (bpp == 15) {
        bpp = 16;
    }

    if (compress) {
        int first_count;
        int second_count;
        int remaining = width;

        while (remaining) {
            first_count = BMP_runcount(getp, remaining);
            second_count = BMP_runcount(getp + first_count, remaining - first_count);

            /* Handle a run of same bytes */
            if (width < 3 || first_count + second_count >= 4) {
                /* Handle end of line */
                if (remaining < 3) {
                    first_count = 1;

                    if (remaining == 2 && getp[0] == getp[1]) {
                        first_count = 2;
                    }
                }

                *putp++ = (uint8_t)first_count;
                *putp++ = *getp;
                getp += first_count;
                remaining -= first_count;
            } else {
                /* Work out how far to the next run of same bytes */
                int third_count;

                for (first_count = 4; first_count < remaining && first_count < 254; first_count += 2) {
                    second_count = BMP_runcount(getp + first_count, remaining - first_count);
                    third_count = BMP_runcount(getp + second_count, remaining - (first_count + second_count));

                    if ((second_count >= 3 && third_count >= 5) || second_count >= 5) {
                        break;
                    }
                }

                if (first_count >= remaining - 2) {
                    first_count = remaining;
                }

                if (first_count > 254) {
                    first_count = 254;
                }

                /* Write out literal run */
                if (first_count >= 3) {
                    *putp++ = 0;
                    *putp++ = (uint8_t)first_count;
                    remaining -= first_count;

                    while (first_count--) {
                        *putp++ = *getp++;
                    }

                    /* Pad literal run if it ends on odd byte */
                    if (((uintptr_t)putp) & 1) {
                        *putp++ = 0;
                    }
                }
            }
        }

        *putp++ = 0;
        *putp++ = 0;
    } else { /* Uncompressed data */
        for (int i = 0; i < width; ++i) {
            switch (bpp) {
                case 32:
                case 16: {
                    uint32_t alpha = (((ARGB *)getp)->a * ((1 << a_bits) - 1) + 128) / 255;
                    uint32_t red = (((ARGB *)getp)->r * ((1 << r_bits) - 1) + 128) / 255;
                    uint32_t green = (((ARGB *)getp)->g * ((1 << g_bits) - 1) + 128) / 255;
                    uint32_t blue = (((ARGB *)getp)->b * ((1 << b_bits) - 1) + 128) / 255;
                    uint32_t packed = alpha << (32 - a_bits) | red << (32 - a_bits - r_bits)
                        | green << (32 - a_bits - r_bits - g_bits) | blue << (32 - a_bits - r_bits - g_bits - b_bits);

                    if (bpp == 32) /* 32 bpp */
                    {
                        *(uint32_t *)putp = htole32(packed);
                        putp += sizeof(uint32_t);
                    } else /* 16 bpp */
                    {
                        *(uint16_t *)putp = htole16((uint16_t)packed);
                        putp += sizeof(uint16_t);
                    }

                    getp += 4;

                    break;
                }
                case 24:
                    putp[0] = ((ARGB *)getp)->b;
                    putp[1] = ((ARGB *)getp)->g;
                    putp[2] = ((ARGB *)getp)->r;
                    putp += 3;
                    getp += 4;
                    break;
                case 8:
                    *putp++ = *getp++;
                    break;
                case 4:
                    if (i & 1) {
                        *putp |= (*getp & 15);
                        ++putp;
                    } else {
                        *putp = (*getp & 15) << 4;
                        if (i == width - 1) {
                            ++putp;
                        }
                    }

                    ++getp;
                    break;
                case 1:
                    if (!(i & 7)) {
                        *putp = 0;
                    }

                    *putp |= (*getp & 1) << (7 - (i & 7));

                    if ((i & 7) == 7 || i == width - 1) {
                        ++putp;
                    }

                    ++getp;
                    break;
            }
        }

        /* Pad to 4 byte alignment */
        while ((uintptr_t)putp & 3) {
            *putp++ = 0;
        }
    }

    return (int)(putp - dst);
}

int GIMEX_API BMP_is(GSTREAM *stream)
{
    BITMAPFILEHEADER header;

    gseek(stream, 0);

    if (gread(stream, &header, sizeof(header.type)) && le16toh(header.type) == BF_TYPE) {
        return 100;
    }

    return 0;
}

int GIMEX_API BMP_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst = galloc(sizeof(GINSTANCE));

    if (inst == NULL) {
        return 0;
    }

    memset(inst, 0, sizeof(GINSTANCE));
    inst->frames = 1;
    inst->signature = GIMEX_ID('.', 'B', 'M', 'P');
    inst->size = sizeof(GINSTANCE);
    inst->stream = stream;
    *ctx = inst;

    return 1;
}

int GIMEX_API BMP_close(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

int GIMEX_API BMP_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
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

int GIMEX_API BMP_wclose(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

GINFO *GIMEX_API BMP_info(GINSTANCE *ctx, int frame)
{
    BITMAPHEADER header;
    GINFO *info = NULL;
    uint32_t header_size = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    int32_t ppm = 0;
    uint32_t colors = 0;
    uint32_t red_mask = 0;
    uint32_t green_mask = 0;
    uint32_t blue_mask = 0;
    uint32_t alpha_mask = 0;
    int32_t type = BMP_UNKNOWN;
    int32_t compression = BI_RGB;

    gseek(ctx->stream, 0);
    memset(&header, 0, sizeof(header));

    /* Get file header and size of bitmap header */
    if (!gread(ctx->stream, &header, sizeof(header.file) + sizeof(header.bmp.size))) {
        return NULL;
    }

    header_size = le32toh(header.bmp.size);

    if (!gread(ctx->stream, &header.bmp.width, header_size - sizeof(header.bmp.size))) {
        return NULL;
    }

    /* We have a core header or some OS/2 header so just use core info */
    if (header_size < sizeof(BITMAPINFOHEADER)) {
        type = BMP_CORE;
        width = le32toh(header.bmp.width);
        height = le32toh(header.bmp.height);
        bpp = le16toh(header.bmp.bit_count);
        colors = 1 << bpp;
    } else { /* We have an info or later header */
        type = BMP_INFO;
        width = le32toh(header.bmp.width);
        height = le32toh(header.bmp.height);
        bpp = le16toh(header.bmp.bit_count);
        colors = le32toh(header.bmp.clr_used);
        compression = le32toh(header.bmp.compression);
        ppm = le32toh(header.bmp.xpels_per_meter);

        /* If we have a basic info header and bitfields, the RGB masks are also actually present */
        if (header_size == sizeof(BITMAPINFOHEADER) && compression == BI_BITFIELDS) {
            type = BMP_INFOEX;
            if (!gread(ctx->stream, &header.bmp.red_mask, sizeof(header.bmp.red_mask) * 3)) {
                return NULL;
            }
        }

        if (header_size == sizeof(BITMAPV4HEADER)) {
            type = BMP_V4;
        }

        red_mask = le32toh(header.bmp.red_mask);
        green_mask = le32toh(header.bmp.green_mask);
        blue_mask = le32toh(header.bmp.blue_mask);
        alpha_mask = le32toh(header.bmp.alpha_mask);
    }

    if (height < 0) {
        height = -height;
    }

    info = (GINFO *)galloc(sizeof(GINFO));

    if (info) {
        memset(info, 0, sizeof(GINFO));
        info->signature = GIMEX_ID('.', 'B', 'M', 'P');
        info->size = sizeof(GINFO);
        info->version = GIMEX_VERSION;
        info->frame_num = 0;
        info->width = width;
        info->height = height;
        info->sub_type = type;
        info->packed = compression == 1 || compression == 2;
        info->quality = 100;
        info->alpha_bits = 0;
        info->red_bits = 8;
        info->green_bits = 8;
        info->blue_bits = 8;
        info->center_x = 0;
        info->center_y = 0;
        info->default_x = 0;
        info->default_y = 0;
        info->num_hotspots = 0;
        info->dpi = ppm / 39.370079f;
        info->frame_name[0] = '\0';

        /* Handle palatted images and none paletted images */
        if (bpp <= 8) {
            uint8_t pal[GIMEX_COLOURTBL_SIZE * sizeof(ARGB)];

            if (bpp == 0) {
                bpp = 8;
            }

            if (colors == 0) {
                colors = 1 << bpp;
            }

            if (colors > 256) {
                colors = 256;
            }

            if (colors > 0) {
                if (type == BMP_CORE) {
                    if (!gread(ctx->stream, pal, colors * 3)) {
                        free(info);
                        return NULL;
                    }

                    for (unsigned i = 0; i < colors; ++i) {
                        info->colortbl[i].a = 255;
                        info->colortbl[i].r = pal[i * 3 + 2];
                        info->colortbl[i].g = pal[i * 3 + 1];
                        info->colortbl[i].b = pal[i * 3];
                    }
                } else {
                    bool no_alpha = true;
                    bool full_alpha = true;

                    if (!gread(ctx->stream, pal, colors * 4)) {
                        free(info);
                        return NULL;
                    }

                    for (unsigned i = 0; i < colors; ++i) {
                        info->colortbl[i].a = pal[i * 4 + 3];
                        info->colortbl[i].r = pal[i * 4 + 2];
                        info->colortbl[i].g = pal[i * 4 + 1];
                        info->colortbl[i].b = pal[i * 4];

                        if (info->colortbl[i].a != 0) {
                            no_alpha = false;
                        }

                        if (info->colortbl[i].a != 255) {
                            full_alpha = false;
                        }
                    }

                    if (no_alpha) {
                        for (unsigned i = 0; i < colors; i++) {
                            info->colortbl[i].a = 255;
                        }

                        info->alpha_bits = 0;
                    } else {
                        info->alpha_bits = full_alpha ? 0 : 8;
                    }
                }

                info->bpp = 8;
                info->original_bpp = bpp;
                info->num_colors = colors;
            }
        } else {
            info->bpp = 32;
            info->original_bpp = bpp;
            info->num_colors = 0;

            if (type == BMP_V4 || type == BMP_INFOEX) {
                if (bpp == 32 && type == BMP_V4) {
                    info->alpha_bits = BMP_getbits(alpha_mask);
                } else {
                    info->alpha_bits = 0;
                }
                info->red_bits = BMP_getbits(red_mask);
                info->green_bits = BMP_getbits(green_mask);
                info->blue_bits = BMP_getbits(blue_mask);
            } else {
                if (bpp == 16) {
                    info->original_bpp = 15;
                    info->red_bits = info->green_bits = info->blue_bits = 5;
                }
            }
        }
    }

    return info;
}

int GIMEX_API BMP_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    BITMAPHEADER header;
    uint8_t *line_buffer = NULL;
    uint32_t header_size = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    int32_t actual_bpp = 0;
    int32_t ppm = 0;
    uint32_t colors = 0;
    uint32_t red_mask = 0;
    uint32_t green_mask = 0;
    uint32_t blue_mask = 0;
    uint32_t alpha_mask = 0;
    int32_t type = BMP_UNKNOWN;
    int32_t compression = BI_RGB;
    int32_t offset;
    int retval = 0;

    colors = info->num_colors;
    type = info->sub_type;
    compression = info->packed;
    width = info->width;
    height = info->height;

    gseek(ctx->stream, 0);

    /* Read required info from header, either core for older formats or the info header */
    if (!gread(ctx->stream,
            &header,
            sizeof(BITMAPFILEHEADER) + (type < BMP_V4 ? sizeof(BITMAPCOREHEADER) : sizeof(BITMAPINFOHEADER) + 16))) {
        return 0;
    }

    bpp = info->original_bpp;

    /* If file format doesn't store the masks, use some standard ones */
    if (type != BMP_INFOEX && type != BMP_V4) {
        if (bpp == 15) {
            red_mask = 0x7C00;
            green_mask = 0x3E0;
            blue_mask = 0x1F;
        } else if (bpp == 32) {
            red_mask = 0xFF0000;
            green_mask = 0xFF00;
            blue_mask = 0xFF;
        }

        alpha_mask = 0;
    } else {
        red_mask = le32toh(header.bmp.red_mask);
        green_mask = le32toh(header.bmp.green_mask);
        blue_mask = le32toh(header.bmp.blue_mask);

        if (bpp == 32 && type == BMP_V4) {
            alpha_mask = le32toh(header.bmp.alpha_mask);
        } else {
            alpha_mask = 0;
        }
    }

    offset = le32toh(header.file.off_bits);

    if (offset == 0) {
        switch (type) {
            case BMP_CORE:
                if (bpp <= 8) {
                    offset = 3 * colors + sizeof(BITMAPCOREHEADER) + sizeof(BITMAPFILEHEADER);
                } else {
                    offset = sizeof(BITMAPCOREHEADER) + sizeof(BITMAPFILEHEADER);
                }
                break;
            case BMP_INFOEX:
                offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 12;
                break;
            case BMP_V4:
                offset = sizeof(BITMAPV4HEADER) + sizeof(BITMAPFILEHEADER);
                break;
            default:
                if (bpp <= 8) {
                    offset = 3 * colors + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
                } else {
                    offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
                }
                break;
        }
    }

    retval = gseek(ctx->stream, offset);
    actual_bpp = bpp != 15 ? bpp : 16;
    line_buffer = galloc(((width * actual_bpp + 31) & ~31) >> 3);

    if (line_buffer == NULL) {
        return 0;
    }

    if (header.bmp.height >= 0) {
        char *putp = buffer + (pitch * (height - 1));
        for (int y = 0; y < height; ++y) {
            if (retval != 0) {
                retval = BMP_readline(
                    ctx->stream, (uint8_t *)putp, line_buffer, bpp, info, alpha_mask, red_mask, green_mask, blue_mask);
            }

            putp -= pitch;
        }
    } else {
        char *putp = buffer;
        for (int y = height; y > 0; --y) {
            if (retval != 0) {
                retval = BMP_readline(
                    ctx->stream, (uint8_t *)putp, line_buffer, bpp, info, alpha_mask, red_mask, green_mask, blue_mask);
            }

            putp += pitch;
        }
    }

    gfree(line_buffer);

    return retval;
}

int GIMEX_API BMP_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    BITMAPHEADER header;
    uint8_t *line_buffer = NULL;
    uint32_t header_size = sizeof(BITMAPINFOHEADER);
    uint32_t header_extra = 0;
    uint32_t compression = BI_RGB;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bpp = 0;
    int32_t dst_pitch = 0;
    uint32_t data_size = 0;
    int32_t alpha_bits = 0;
    int32_t red_bits = 0;
    int32_t green_bits = 0;
    int32_t blue_bits = 0;
    int32_t num_colors = info->num_colors;
    int retval = 0;

    width = info->width;
    height = info->height;

    if (info->bpp <= 8) {
        bpp = info->original_bpp <= 8 ? info->original_bpp : 8;
    } else {
        bpp = info->alpha_bits + info->blue_bits + info->green_bits + info->red_bits;
    }

    /* Sanity check the bpp */
    if (bpp <= 1) {
        bpp = 1;
    } else if (bpp <= 4) {
        bpp = 4;
    } else if (bpp <= 8) {
        bpp = 8;
    } else if (bpp <= 16) {
        bpp = 16;
    } else if (bpp <= 24) {
        bpp = 24;
    } else {
        bpp = 32;
    }

    /* No palette entries if we are targetting higher bit depths */
    if (bpp > 8) {
        num_colors = 0;
    }

    memset(&header, 0, sizeof(header));
    alpha_bits = info->alpha_bits;
    red_bits = info->red_bits;
    green_bits = info->green_bits;
    blue_bits = info->blue_bits;

    if (info->sub_type == BMP_V4) {
        header_size = sizeof(BITMAPV4HEADER);

        if (info->alpha_bits == 0) {
            if (bpp == 16 && info->red_bits <= 5 && info->green_bits <= 6 && info->blue_bits <= 5) {
                compression = BI_BITFIELDS;

                if (info->green_bits <= 5) {
                    red_bits = 5;
                    green_bits = 5;
                    blue_bits = 5;
                    alpha_bits = 1;
                    header.bmp.red_mask = htole32(0x7C00);
                    header.bmp.green_mask = htole32(0x03E0);
                    header.bmp.blue_mask = htole32(0x001F);
                    header.bmp.alpha_mask = htole32(0x0000);
                } else {
                    red_bits = 5;
                    green_bits = 6;
                    blue_bits = 5;
                    alpha_bits = 0;
                    header.bmp.red_mask = htole32(0xF800);
                    header.bmp.green_mask = htole32(0x07E0);
                    header.bmp.blue_mask = htole32(0x001F);
                    header.bmp.alpha_mask = htole32(0x0000);
                }
            } else if (bpp == 24) {
                red_bits = 8;
                green_bits = 8;
                blue_bits = 8;
                alpha_bits = 0;
                header.bmp.red_mask = htole32(0x00FF0000);
                header.bmp.green_mask = htole32(0x0000FF00);
                header.bmp.blue_mask = htole32(0x000000FF);
                header.bmp.alpha_mask = htole32(0x00000000);
            }
        } else if (bpp >= 8) { /* Any alpha means we need to go 32bit */
            bpp = 32;
            red_bits = 8;
            green_bits = 8;
            blue_bits = 8;
            alpha_bits = 8;
            header.bmp.red_mask = htole32(0x00FF0000);
            header.bmp.green_mask = htole32(0x0000FF00);
            header.bmp.blue_mask = htole32(0x000000FF);
            header.bmp.alpha_mask = htole32(0xFF000000);
        }

        header.bmp.cs_type = htole32(1);
        header.bmp.end_points.red.x = 0;
        header.bmp.end_points.red.y = 0;
        header.bmp.end_points.red.z = 0;
        header.bmp.end_points.green.x = 0;
        header.bmp.end_points.green.y = 0;
        header.bmp.end_points.green.z = 0;
        header.bmp.end_points.blue.x = 0;
        header.bmp.end_points.blue.y = 0;
        header.bmp.end_points.blue.z = 0;
        header.bmp.gamma_red = 0;
        header.bmp.gamma_green = 0;
        header.bmp.gamma_blue = 0;
    } else if (info->sub_type == BMP_INFOEX) {
        compression = BI_BITFIELDS;
        header_extra = 12; /* Bit masks not counted in header size */

        /* Check if the needed bits exceed the desired number */
        if (bpp == 16) {
            if (red_bits + green_bits + blue_bits + alpha_bits > 16) {
                bpp = 32;
            }
        }

        alpha_bits = alpha_bits < 8 ? alpha_bits : 8;
        red_bits = red_bits < 8 ? red_bits : 8;
        green_bits = green_bits < 8 ? green_bits : 8;
        blue_bits = blue_bits < 8 ? blue_bits : 8;

        header.bmp.alpha_mask = htole32(((1 << alpha_bits) - 1) << (red_bits + green_bits + blue_bits));
        header.bmp.red_mask = htole32(((1 << red_bits) - 1) << (green_bits + blue_bits));
        header.bmp.green_mask = htole32(((1 << green_bits) - 1) << blue_bits);
        header.bmp.blue_mask = htole32((1 << blue_bits) - 1);
    } else {
        if (bpp == 16) {
            if (info->sub_type == BMP_INFO) {
                red_bits = 5;
                green_bits = 6;
                blue_bits = 5;
                alpha_bits = 0;
            } else {
                red_bits = 5;
                green_bits = 5;
                blue_bits = 5;
                alpha_bits = 1;
            }
        } else if (bpp == 32) {
            red_bits = 8;
            green_bits = 8;
            blue_bits = 8;
            alpha_bits = 8;
        } else if (bpp == 8) {
            if (info->packed) {
                compression = BI_RLE8;
            }
        }
    }

    /* Lines are padded to 32 bit aligned */
    dst_pitch = ((width * bpp + 31) & ~31) / 8;
    line_buffer = galloc(3 * pitch + 28);

    /* We do this once here to work out image size, then again to write it out */
    if (line_buffer != NULL) {
        for (int y = 0; y < info->height; ++y) {
            /* Bitmaps are written bottom row first */
            char *getp = buffer + (info->height - y - 1) * pitch;
            dst_pitch = BMP_writeline((uint8_t *)getp,
                line_buffer,
                width,
                bpp,
                info->sub_type,
                info->packed,
                alpha_bits,
                red_bits,
                green_bits,
                blue_bits);
            data_size += dst_pitch;
        }
    }

    header.file.type = htole16(BF_TYPE);
    header.file.size = htole32(sizeof(BITMAPFILEHEADER) + header_size + header_extra + 4 * num_colors + data_size);
    header.file.reserved1 = 0;
    header.file.reserved2 = 0;
    header.file.off_bits = htole32(sizeof(BITMAPFILEHEADER) + header_size + header_extra + 4 * num_colors);
    header.bmp.size = htole32(header_size);
    header.bmp.width = htole32(width);
    header.bmp.height = htole32(info->height);
    header.bmp.planes = htole32(1);
    header.bmp.bit_count = htole32(bpp);
    header.bmp.compression = htole32(compression);
    header.bmp.size_image = htole32(data_size);
    header.bmp.xpels_per_meter = htole32((uint32_t)(info->dpi * 39.370079f + 0.5f));
    header.bmp.ypels_per_meter = htole32((uint32_t)(info->dpi * 39.370079f + 0.5f));
    header.bmp.clr_used = htole32(num_colors);
    header.bmp.clr_important = 0;

    retval = gwrite(ctx->stream, &header, sizeof(BITMAPFILEHEADER) + header_size + header_extra);

    /* Write the palette if required */
    if (bpp <= 8 && num_colors) {
        uint8_t pal[256 * 4];

        for (int i = 0; i < num_colors; ++i) {
            pal[i * 4 + 0] = info->colortbl[i].b;
            pal[i * 4 + 1] = info->colortbl[i].g;
            pal[i * 4 + 2] = info->colortbl[i].r;

            if (info->alpha_bits != 0) {
                pal[i * 4 + 3] = info->colortbl[i].a;
            } else {
                pal[i * 4 + 3] = 0xFF;
            }
        }

        retval = gwrite(ctx->stream, pal, num_colors * 4);
    }

    /* We do this second time here and actually write to our file/stream */
    if (line_buffer != NULL) {
        for (int y = 0; y < info->height; ++y) {
            /* Bitmaps are written bottom row first */
            char *getp = buffer + (info->height - y - 1) * pitch;
            dst_pitch = BMP_writeline((uint8_t *)getp,
                line_buffer,
                width,
                bpp,
                info->sub_type,
                info->packed,
                alpha_bits,
                red_bits,
                green_bits,
                blue_bits);
            retval = gwrite(ctx->stream, line_buffer, dst_pitch);
        }
    }

    return retval;
}

GABOUT *GIMEX_API BMP_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID('.', 'B', 'M', 'P');
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
        about->mac_type[0] = GIMEX_ID('B', 'I', 'N', 'A');
        about->mac_type[1] = GIMEX_ID('.', 'B', 'M', 'P');
        strcpy(about->extensions[0], ".bmp");
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "BMP");
        strcpy(about->word_type, "BMP");
        strcpy(about->long_type, "Windows Bitmap");
    }

    return about;
}
