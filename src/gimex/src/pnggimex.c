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
#include "pnggimex.h"
#include <png.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct PngContext
{
    png_structp png_ptr;
    png_infop info_ptr;
};

/* Some static functions for interfacing with libpng */
static void PNG_warning(png_structp png_ptr, png_const_charp msg) {}

static png_voidp PNG_malloc(png_structp png_ptr, png_alloc_size_t size)
{
    return galloc(size);
}

static void PNG_free(png_structp png_ptr, png_voidp ptr)
{
    gfree(ptr);
}

static void PNG_read_data(png_structp png_ptr, png_bytep buff, size_t length)
{
    png_voidp stream = png_get_io_ptr(png_ptr);

    if (gread(stream, buff, length) != length) {
        png_error(png_ptr, "Read Error");
    }
}

static void PNG_write_data(png_structp png_ptr, png_bytep buff, size_t length)
{
    png_voidp stream = png_get_io_ptr(png_ptr);

    if (gwrite(stream, buff, length) != length) {
        png_error(png_ptr, "Read Error");
    }
}

static void PNG_flush_data(png_structp png_ptr) {}

static int PNG_read_gimex(png_structp png_ptr, png_infop info_ptr, GINFO *info, char *buffer, int pitch)
{
    png_bytepp rows;

    if (setjmp(png_jmpbuf(png_ptr))) {
        return 0;
    }

    rows = galloc(info->height * sizeof(*rows));

    if (rows != NULL) {
        if (info->bpp == 32) {
            size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
            png_bytep row_data = galloc(row_bytes * info->height);

            if (row_data != NULL) {
                /* Set up pointers for writing to our temporary buffer */
                for (int i = 0; i < info->height; ++i) {
                    rows[i] = (png_byte *)&row_data[row_bytes * i];
                }

                png_read_image(png_ptr, rows);

                for (int i = 0, buff_pos = 0; i < info->height; ++i) {
                    png_bytep put_ptr = (png_bytep)&buffer[buff_pos];
                    png_bytep get_ptr = rows[i];
                    int ctype = png_get_color_type(png_ptr, info_ptr);

                    if (ctype == 2) {
                        for (int j = 0; j < info->width; ++j) {
                            put_ptr[0] = get_ptr[2];
                            put_ptr[1] = get_ptr[1];
                            put_ptr[2] = get_ptr[0];
                            put_ptr[3] = -1;
                            put_ptr += 4;
                            get_ptr += 3;
                        }
                    } else if (ctype == 6) {
                        for (int j = 0; j < info->width; ++j) {
                            put_ptr[0] = get_ptr[2];
                            put_ptr[1] = get_ptr[1];
                            put_ptr[2] = get_ptr[0];
                            put_ptr[3] = get_ptr[3];
                            put_ptr += 4;
                            get_ptr += 4;
                        }
                    }

                    buff_pos += pitch;
                }

                /* Clean up our data buffer */
                gfree(row_data);
            }
        } else {
            /* Set up pointers for direct write to our buffer */
            for (int i = 0; i < info->height; ++i) {
                rows[i] = (png_bytep)&buffer[i * pitch];
            }

            png_read_image(png_ptr, rows);
        }

        /* Clean up our pointer array */
        gfree(rows);
    }

    return 1;
}

static int PNG_write_gimex(png_structp png_ptr, png_infop info_ptr, const GINFO *info, const char *buffer, int pitch)
{
    png_bytepp rows;
    png_bytep row_buff;
    png_bytep row_ptr;
    int row_byte_count;
    png_byte color_type;

    if (setjmp(png_jmpbuf(png_ptr))) {
        return 0;
    }

    rows = galloc(info->height * sizeof(*rows));

    if (rows == NULL) {
        return 1;
    }

    row_byte_count = info->width;
    color_type = png_get_color_type(png_ptr, info_ptr);

    if (info->bpp == 32) {
        if (color_type & 4) {
            row_byte_count *= 4;
        } else {
            row_byte_count *= 3;
        }
    } else if (color_type & 4) {
        row_byte_count *= 2;
    }

    row_buff = row_ptr = galloc(row_byte_count * info->height);

    if (row_buff != NULL) {
        int row_pos = 0;

        /* Set our row pointers */
        for (int i = 0; i < info->height; ++i) {
            rows[i] = row_ptr;
            row_ptr += row_byte_count;
        }

        /* Populate the rows with data */
        for (int i = 0; i < info->height; ++i) {
            const char *get_ptr = &buffer[i * pitch];
            png_bytep put_ptr = rows[i];

            if (info->bpp == 32) {
                if (color_type & 4) {
                    for (int j = 0; j < info->width; ++j) {
                        put_ptr[0] = get_ptr[2];
                        put_ptr[1] = get_ptr[1];
                        put_ptr[2] = get_ptr[0];
                        put_ptr[3] = get_ptr[3];

                        get_ptr += 4;
                        put_ptr += 4;
                    }
                } else {
                    for (int j = 0; j < info->width; ++j) {
                        put_ptr[0] = get_ptr[2];
                        put_ptr[1] = get_ptr[1];
                        put_ptr[2] = get_ptr[0];

                        get_ptr += 4;
                        put_ptr += 3;
                    }
                }
            } else if (color_type == 0 || color_type == 4) {
                int depth = info->original_bpp;
                int div = 1;
                int mult = 0;

                if (depth < 1) {
                    depth = 1;
                }

                if (depth > 8) {
                    depth = 8;
                }

                if (depth != 1) {
                    div = (1 << depth) - 1;
                    mult = 1 << depth >> 1;
                }

                for (int j = 0; j < info->width; ++j) {
                    *put_ptr++ = (mult + 255 * (unsigned char)get_ptr[0]) / div;

                    if (color_type & 4) {
                        *put_ptr++ = info->colortbl[(unsigned char)get_ptr[3]].a;
                    }

                    ++get_ptr;
                }
            } else {
                for (int j = 0; j < info->width; ++j) {
                    put_ptr[j] = get_ptr[j];
                }
            }
        }

        png_write_image(png_ptr, rows);
        gfree(row_buff);
    }

    gfree(rows);

    return 1;
}

/* libpng GIMEX interface */
int GIMEX_API PNG_is(GSTREAM *stream)
{
    char buff[8];

    gseek(stream, 0);
    gread(stream, buff, 8);

    if (png_sig_cmp(buff, 0, 8) == 0) {
        return 100;
    }

    return png_sig_cmp(buff, 0, 4) == 0 ? 50 : 0;
}

int GIMEX_API PNG_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst = galloc(sizeof(GINSTANCE));
    struct PngContext *png_ctx;

    if (inst == NULL) {
        return 0;
    }

    memset(inst, 0, sizeof(GINSTANCE));
    png_ctx = galloc(sizeof(struct PngContext));
    inst->image_context = png_ctx;

    if (png_ctx != NULL) {
        png_ctx->png_ptr =
            png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, PNG_warning, NULL, PNG_malloc, PNG_free);

        if (png_ctx->png_ptr != NULL) {
            png_ctx->info_ptr = png_create_info_struct(png_ctx->png_ptr);

            if (png_ctx->info_ptr != NULL) {
                png_set_read_fn(png_ctx->png_ptr, stream, PNG_read_data);

                if (!setjmp(png_jmpbuf(png_ctx->png_ptr))) {
                    gseek(stream, 0);
                    png_read_info(png_ctx->png_ptr, png_ctx->info_ptr);
                    inst->frames = 1;
                    inst->frame_num = 0;
                    inst->signature = GIMEX_ID(0, 'P', 'N', 'G');
                    inst->size = sizeof(GINSTANCE);
                    inst->stream = stream;
                    *ctx = inst;
                    return true;
                }

                png_destroy_read_struct(&png_ctx->png_ptr, &png_ctx->info_ptr, NULL);
            }

            png_destroy_read_struct(&png_ctx->png_ptr, NULL, NULL);
        }

        gfree(png_ctx);
    }

    gfree(inst);

    return false;
}

int GIMEX_API PNG_close(GINSTANCE *ctx)
{
    if (ctx == NULL) {
        return 0;
    }

    struct PngContext *png_ctx = ctx->image_context;

    if (png_ctx != NULL) {
        png_destroy_read_struct(&png_ctx->png_ptr, &png_ctx->info_ptr, NULL);
        gfree(png_ctx);
    }

    return gfree(ctx);
}

int GIMEX_API PNG_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
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

int GIMEX_API PNG_wclose(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

GINFO *GIMEX_API PNG_info(GINSTANCE *ctx, int frame)
{
    struct PngContext *png_ctx = ctx->image_context;
    GINFO *info = galloc(sizeof(GINFO));

    if (info != NULL) {
        png_infop info_ptr = png_ctx->info_ptr;
        int src_bitdepth;
        int color_bits;
        int text_size;
        unsigned pixels_per_meter;
        memset(info, 0, sizeof(GINFO));

        switch (png_get_color_type(png_ctx->png_ptr, png_ctx->info_ptr)) {
            case PNG_COLOR_TYPE_GRAY:
                src_bitdepth = png_get_bit_depth(png_ctx->png_ptr, png_ctx->info_ptr);
                color_bits = src_bitdepth;
                if (src_bitdepth > 8) {
                    src_bitdepth = 8;
                }
                break;
            default:
                src_bitdepth = 32;
                color_bits = png_get_bit_depth(png_ctx->png_ptr, png_ctx->info_ptr);
                break;
            case PNG_COLOR_TYPE_RGB:
                src_bitdepth = 24;
                color_bits = png_get_bit_depth(png_ctx->png_ptr, png_ctx->info_ptr);
                break;
            case PNG_COLOR_TYPE_PALETTE:
                src_bitdepth = png_get_bit_depth(png_ctx->png_ptr, png_ctx->info_ptr);
                if (src_bitdepth > 8) {
                    src_bitdepth = 8;
                }
                color_bits = 8;
                break;
        }

        info->signature = GIMEX_ID(0, 'P', 'N', 'G');
        info->size = sizeof(GINFO);
        info->version = GIMEX_VERSION;
        info->frame_num = 0;
        info->width = png_get_image_width(png_ctx->png_ptr, png_ctx->info_ptr);
        info->height = png_get_image_height(png_ctx->png_ptr, png_ctx->info_ptr);
        info->frame_size = 0;
        info->sub_type = 0;
        info->packed = 0;
        info->quality = 100;
        info->alpha_bits = (png_get_color_type(png_ctx->png_ptr, png_ctx->info_ptr) & 4) != 0 ? color_bits : 0;
        info->red_bits = color_bits;
        info->green_bits = color_bits;
        info->blue_bits = color_bits;
        info->center_x = 0;
        info->center_y = 0;
        info->default_x = 0;
        info->default_y = 0;
        info->num_hotspots = 0;
        info->frame_name[0] = '\0';
        info->bpp = src_bitdepth > 8 ? 32 : src_bitdepth;
        info->original_bpp = src_bitdepth;
        info->num_colors = 0;
        png_get_text(png_ctx->png_ptr, png_ctx->info_ptr, (png_textp *)info->comment, &text_size);
        pixels_per_meter = png_get_x_pixels_per_meter(png_ctx->png_ptr, png_ctx->info_ptr);
        info->dpi = pixels_per_meter * 0.039370079f;

        if (src_bitdepth <= 8) {
            if (png_get_color_type(png_ctx->png_ptr, png_ctx->info_ptr) != 0) {
                png_colorp palettes;

                if (png_get_PLTE(png_ctx->png_ptr, png_ctx->info_ptr, &palettes, &info->num_colors) == PNG_INFO_PLTE
                    && info->num_colors != 0) {
                    png_bytep trans_alpha = NULL;
                    png_color_16p trans_color = NULL;
                    int num_trans = 0;
                    if (png_get_tRNS(png_ctx->png_ptr, png_ctx->info_ptr, &trans_alpha, &num_trans, &trans_color)
                        == PNG_INFO_tRNS) {
                        info->alpha_bits = 8;
                    } else {
                        trans_alpha = NULL;
                    }

                    for (int i = 0; i < info->num_colors; ++i) {
                        info->colortbl[i].r = palettes[i].red;
                        info->colortbl[i].g = palettes[i].green;
                        info->colortbl[i].b = palettes[i].blue;

                        if (trans_alpha != NULL && i < num_trans) {
                            info->colortbl[i].a = trans_alpha[i];
                        } else {
                            info->colortbl[i].a = 255;
                        }
                    }
                } else {
                    gfree(info);
                    info = NULL;
                }
            } else {
                info->num_colors = 1 << src_bitdepth;

                for (int i = 0, j = 0; i < info->num_colors; ++i) {
                    int val = j / (info->num_colors - 1);

                    info->colortbl[i].a = 255;
                    info->colortbl[i].r = val;
                    info->colortbl[i].g = val;
                    info->colortbl[i].b = val;
                    j += 255;
                }
            }
        }
    }

    return info;
}

int GIMEX_API PNG_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    png_structp png_ptr =
        png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, PNG_warning, NULL, PNG_malloc, PNG_free);
    png_infop info_ptr;
    int read = 0;

    if (png_ptr == NULL) {
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr != NULL) {
        png_set_read_fn(png_ptr, ctx->stream, PNG_read_data);

        if (!setjmp(png_jmpbuf(png_ptr))) {
            gseek(ctx->stream, 0);
            png_read_info(png_ptr, info_ptr);
        }

        if (png_get_bit_depth(png_ptr, info_ptr) == 16) {
            png_set_strip_16(png_ptr);
        }

        if (png_get_bit_depth(png_ptr, info_ptr) < 8) {
            png_set_packing(png_ptr);
        }

        if (png_get_color_type(png_ptr, info_ptr) == 4) {
            png_set_gray_to_rgb(png_ptr);
        }

        png_read_update_info(png_ptr, info_ptr);
        read = PNG_read_gimex(png_ptr, info_ptr, info, buffer, pitch);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return read;
}

int GIMEX_API PNG_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    png_structp png_ptr;

    if (ctx->frame_num) {
        return 0;
    }

    png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, PNG_warning, NULL, PNG_malloc, PNG_free);

    if (png_ptr != NULL) {
        png_infop info_ptr = png_create_info_struct(png_ptr);

        if (info_ptr != NULL) {
            png_set_write_fn(png_ptr, ctx->stream, PNG_write_data, PNG_flush_data);

            if (!setjmp(png_jmpbuf(png_ptr))) {
                png_colorp palette = NULL;
                png_bytep trans_pal = NULL;
                int color_bits;
                int color_type;
                int written = 0;

                if (info->bpp == 32) {
                    if (info->alpha_bits != 0) {
                        color_bits = 8;
                        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
                    } else {
                        color_bits = 8;
                        color_type = PNG_COLOR_TYPE_RGB;
                    }
                } else {
                    int src_depth = info->original_bpp;
                    bool grey_scale;

                    if (src_depth != 1 && src_depth != 2) {
                        src_depth = (uint8_t)(4 * (src_depth > 4) + 4);
                    }

                    grey_scale = true;

                    for (int i = 0; i < info->num_colors; ++i) {
                        if (info->colortbl[i].r != info->colortbl[i].g || info->colortbl[i].r != info->colortbl[i].b) {
                            grey_scale = false;
                            break;
                        }
                    }

                    if (!grey_scale) {
                        palette = galloc(sizeof(png_color) * info->num_colors);
                        png_colorp pal_ptr = palette;

                        for (int i = 0; i < info->num_colors; ++i) {
                            pal_ptr[i].red = info->colortbl[i].r;
                            pal_ptr[i].green = info->colortbl[i].g;
                            pal_ptr[i].blue = info->colortbl[i].b;
                        }

                        color_bits = src_depth;
                        color_type = PNG_COLOR_TYPE_PALETTE;
                    } else {
                        if (info->alpha_bits != 0) {
                            color_bits = 8;
                            color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
                        } else {
                            color_bits = src_depth;
                            color_type = PNG_COLOR_TYPE_GRAY;
                        }
                    }
                }

                png_set_IHDR(png_ptr,
                    info_ptr,
                    info->width,
                    info->height,
                    color_bits,
                    color_type,
                    PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT,
                    PNG_FILTER_TYPE_DEFAULT);

                if (color_type == PNG_COLOR_TYPE_PALETTE) {
                    png_set_PLTE(png_ptr, info_ptr, palette, info->num_colors);

                    if (info->alpha_bits > 0) {
                        int num_palette;
                        png_get_PLTE(png_ptr, info_ptr, NULL, &num_palette);
                        trans_pal = galloc(4 * num_palette);

                        for (int i = 0; i < num_palette; ++i) {
                            trans_pal[i] = info->colortbl[i].a;
                        }
                        png_set_tRNS(png_ptr, info_ptr, trans_pal, num_palette, NULL);
                    }
                }

                if (info->dpi != 0.0f) {
                    png_set_pHYs(png_ptr,
                        info_ptr,
                        (png_uint_32)(info->dpi / 0.039370079f),
                        (png_uint_32)(info->dpi / 0.039370079f),
                        1);
                }

                png_write_info(png_ptr, info_ptr);

                if (png_get_bit_depth(png_ptr, info_ptr) < 8) {
                    png_set_packing(png_ptr);
                }

                written = PNG_write_gimex(png_ptr, info_ptr, info, buffer, pitch);
                png_write_end(png_ptr, info_ptr);

                if (palette != NULL) {
                    gfree(palette);
                }

                if (trans_pal != NULL) {
                    gfree(trans_pal);
                }
            }
        }

        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    ++ctx->frame_num;
    return 0;
}

GABOUT *GIMEX_API PNG_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID(0, 'P', 'N', 'G');
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
        about->multi_size = 0;
        about->requires_frame_buffer = 0;
        about->external = 0;
        about->uses_file = 1;
        about->max_frame_name = 0;
        about->default_quality = 100;
        about->mac_type[0] = GIMEX_ID(0, 'P', 'N', 'G');
        strcpy(about->extensions[0], ".png");
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "PNG");
        strcpy(about->word_type, "PNG");
        strcpy(about->long_type, "Portable Network Graphics");
    }

    return about;
}
