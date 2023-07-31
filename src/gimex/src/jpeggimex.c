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
#include "jpeggimex.h"
#include "jpegio.h"
#include <endianness.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <jerror.h>

/* Custom error handling for libjpeg */

struct gimex_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

static void gimex_error_exit(j_common_ptr cinfo)
{
    struct gimex_error_mgr *myerr = (struct gimex_error_mgr *)cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}

/* Custom marker handler for gimex written JPEG files */

/* Define the marker GIMEX will use in the jpeg standard */
#define JPEG_APP13 0xED

static int JPG_readgimexmarker;
#define GIMEXMARKER "GIMEXARGB"
#define GIMEXMARKERSIZE (sizeof(GIMEXMARKER) - 1)

/* Helper functions for parsing the JPEG file */
static void JPG_readcmyk(uint8_t cmyk[4], uint8_t rgb[3])
{
    rgb[0] = (uint8_t)(255 - gmin(cmyk[0] + cmyk[3], 255));
    rgb[1] = (uint8_t)(255 - gmin(cmyk[1] + cmyk[3], 255));
    rgb[2] = (uint8_t)(255 - gmin(cmyk[2] + cmyk[3], 255));
}

/*
 * Binary Match MSVC 2003 cl.exe -O1 -GF -Gm- -Oy-
 */
static void JPG_readline(const uint8_t *src, uint8_t *dst, int width, int channels, J_COLOR_SPACE cspace, int gimex_format)
{
    const uint8_t *get_ptr;
    uint8_t tmp[3];
    ARGB *put_ptr;
    int i;

    get_ptr = src;
    put_ptr = (ARGB *)dst;

    if (gimex_format) {
        for (i = 0; i < width; ++i) {
            put_ptr[i].a = get_ptr[i * 4 + 0];
            put_ptr[i].r = get_ptr[i * 4 + 1];
            put_ptr[i].g = get_ptr[i * 4 + 2];
            put_ptr[i].b = get_ptr[i * 4 + 3];
        }
    } else {
        switch (cspace) {
            default:
            case JCS_RGB:
                for (i = 0; i < width; ++i) {
                    put_ptr[i].a = 255;
                    put_ptr[i].r = get_ptr[i * 3 + 0];
                    put_ptr[i].g = get_ptr[i * 3 + 1];
                    put_ptr[i].b = get_ptr[i * 3 + 2];
                }
                break;

            case JCS_CMYK:
                for (i = 0; i < width; ++i) {
                    JPG_readcmyk((uint8_t *)&get_ptr[i * 4], tmp);

                    put_ptr[i].a = 255;
                    put_ptr[i].r = tmp[0];
                    put_ptr[i].g = tmp[1];
                    put_ptr[i].b = tmp[2];
                }

                break;
            case JCS_GRAYSCALE:
                memcpy(dst, src, width);
                break;
        }
    }
}

/*
 * Inlined within JPG_read
 */
static void JPG_selectoutputcolourspace(struct jpeg_decompress_struct *cinfo)
{
    if (cinfo->jpeg_color_space == JCS_YCbCr) {
        cinfo->out_color_space = JCS_RGB;
    } else if (cinfo->jpeg_color_space == JCS_YCCK) {
        cinfo->out_color_space = JCS_CMYK;
    }
}

/*
 * Binary Match MSVC 2003 cl.exe -O1 -GF -Gm- -Oy-
 */
static boolean JPG_markerparser(j_decompress_ptr cinfo)
{
    int length;
    char read_marker[GIMEXMARKERSIZE];
    int i;
    struct jpeg_source_mgr *src = cinfo->src;
    const JOCTET *byte = src->next_input_byte;
    size_t bytes_in_buff = src->bytes_in_buffer;

    if (bytes_in_buff == 0) {
        if (!src->fill_input_buffer(cinfo)) {
            return 0;
        }

        byte = src->next_input_byte;
        bytes_in_buff = src->bytes_in_buffer;
    }

    bytes_in_buff--;
    length = (*byte++) << 8;

    if (bytes_in_buff == 0) {
        if (!src->fill_input_buffer(cinfo)) {
            return 0;
        }

        byte = src->next_input_byte;
        bytes_in_buff = src->bytes_in_buffer;
    }

    bytes_in_buff--;
    length += *byte++;
    length -= 2;

    if (length >= GIMEXMARKERSIZE) {
        for (i = 0; i < sizeof(read_marker); i++) {
            if (bytes_in_buff == 0) {
                if (!src->fill_input_buffer(cinfo)) {
                    return 0;
                }

                byte = src->next_input_byte;
                bytes_in_buff = src->bytes_in_buffer;
            }

            --bytes_in_buff;
            read_marker[i] = *byte++;
        }

        length -= 9;
    }

    if (strncmp(read_marker, GIMEXMARKER, GIMEXMARKERSIZE) == 0) {
        JPG_readgimexmarker = 1;
    }

    src->next_input_byte = byte;
    src->bytes_in_buffer = bytes_in_buff;

    if (length > 0) {
        cinfo->src->skip_input_data(cinfo, length);
    }

    return 1;
}

void JPG_writeline(const uint8_t *src, uint8_t *dst, int width, J_COLOR_SPACE cspace, const GINFO *info)
{
    int i;

    switch (cspace) {
        case JCS_GRAYSCALE:
            if (info->bpp == 32) {
                for (i = 0; i < width; ++i) {
                    dst[i] = ((ARGB *)src)[i].g;
                }
            } else {
                for (i = 0; i < width; ++i) {
                    dst[i] = info->colortbl[src[i]].g;
                }
            }
            break;
        case JCS_RGB:
            if (info->bpp == 32) {
                for (i = 0; i < width; ++i) {
                    dst[3 * i] = ((ARGB *)src)[i].r;
                    dst[3 * i + 1] = ((ARGB *)src)[i].g;
                    dst[3 * i + 2] = ((ARGB *)src)[i].b;
                }
            } else {
                for (i = 0; i < width; ++i) {
                    dst[3 * i] = info->colortbl[src[i]].r;
                    dst[3 * i + 1] = info->colortbl[src[i]].g;
                    dst[3 * i + 2] = info->colortbl[src[i]].b;
                }
            }
            break;
        case JCS_UNKNOWN:
            if (info->bpp == 32) {
                for (i = 0; i < width; ++i) {
                    dst[4 * i] = ((ARGB *)src)[i].a;
                    dst[4 * i + 1] = ((ARGB *)src)[i].r;
                    dst[4 * i + 2] = ((ARGB *)src)[i].g;
                    dst[4 * i + 3] = ((ARGB *)src)[i].b;
                }
            } else {
                for (i = 0; i < width; ++i) {
                    dst[4 * i] = info->colortbl[src[i]].a;
                    dst[4 * i + 1] = info->colortbl[src[i]].r;
                    dst[4 * i + 2] = info->colortbl[src[i]].g;
                    dst[4 * i + 3] = info->colortbl[src[i]].b;
                }
            }
            break;
        default:
            break;
    }
}

/* GIMEX interface implementations */

/*
 * Binary Match MSVC 2003 cl.exe -O1 -GF -Gm- -Oy-
 */
int GIMEX_API JPG_is(GSTREAM *stream)
{
    uint8_t file_id[4];
    int retval = 0;

    gseek(stream, 0, GSEEK_SET);

    if (gread(stream, &file_id, sizeof(file_id))) {
        if (get_be32(file_id) == 0xFFD8FFE0 || get_be32(file_id) == 0xFFD8FFE1 || get_be32(file_id) == 0xFFD8FFED) {
            retval = 99;
        }
    }

    return retval;
}

int GIMEX_API JPG_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst;
    int retval = 0;

    inst = galloc(sizeof(GINSTANCE));

    if (inst != NULL) {
        memset(inst, 0, sizeof(GINSTANCE));
        inst->frames = 1;
        inst->signature = GIMEX_ID('J', 'P', 'E', 'G');
        inst->size = sizeof(GINSTANCE);
        inst->stream = stream;
        *ctx = inst;
        retval = 1;
    }

    return retval;
}

int GIMEX_API JPG_close(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

int GIMEX_API JPG_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
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

int GIMEX_API JPG_wclose(GINSTANCE *ctx)
{
    return ctx != NULL ? gfree(ctx) : 0;
}

GINFO *GIMEX_API JPG_info(GINSTANCE *ctx, int frame)
{
    GINFO *info;
    int i;
    struct jpeg_decompress_struct cinfo;
    struct gimex_error_mgr jerr;
    JSAMPARRAY row_buff;
    int bytes_per_pixel;
    int width;
    int height;
    int density;
    int bitdepth;
    bool gimex_marker;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = gimex_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return 0;
    }

    /* Read header info */
    jpeg_create_decompress(&cinfo);
    jpeg_set_marker_processor(&cinfo, JPEG_APP13, JPG_markerparser);
    gseek(ctx->stream, 0, GSEEK_SET);
    gimex_stream_src(&cinfo, ctx->stream);
    JPG_readgimexmarker = false;
    jpeg_read_header(&cinfo, TRUE);
    gimex_marker = JPG_readgimexmarker;
    jpeg_start_decompress(&cinfo);

    bytes_per_pixel = cinfo.output_components;
    width = cinfo.output_width;
    height = cinfo.output_height;
    density = cinfo.density_unit ? cinfo.X_density : 0;
    row_buff = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, 1, cinfo.output_components * cinfo.output_width, 1);

    /* Read rest of file and drop it */
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, row_buff, 1);
    }

    /* Close the context */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    bitdepth = bytes_per_pixel == 1 ? 8 : 24;

    info = galloc(sizeof(GINFO));

    if (info) {
        memset(info, 0, sizeof(GINFO));
        info->signature = GIMEX_ID('J', 'P', 'E', 'G');
        info->size = sizeof(GINFO);
        info->version = GIMEX_VERSION;
        info->frame_num = 0;
        info->width = width;
        info->height = height;
        info->sub_type = gimex_marker != false;
        info->packed = 1;
        info->quality = 70;
        info->alpha_bits = 0;
        info->red_bits = 8;
        info->green_bits = 8;
        info->blue_bits = 8;
        info->center_x = 0;
        info->center_y = 0;
        info->default_x = 0;
        info->default_y = 0;
        info->num_hotspots = 0;
        info->dpi = (float)density;
        info->frame_name[0] = '\0';
        if (bitdepth <= 14) {
            info->bpp = 8;
            info->original_bpp = 8;
            info->num_colors = 256;

            for (i = 0; i != 256; ++i) {
                info->colortbl[i].a = 255;
                info->colortbl[i].r = (uint8_t)i;
                info->colortbl[i].g = (uint8_t)i;
                info->colortbl[i].b = (uint8_t)i;
            }
        } else {
            info->bpp = 32;
            info->original_bpp = bitdepth;
            info->num_colors = 0;
        }
    }

    return info;
}

int GIMEX_API JPG_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    int i;
    int width;
    int height;
    int channels;
    int retval = 1;
    struct jpeg_decompress_struct cinfo;
    struct gimex_error_mgr jerr;
    JSAMPARRAY row_buff;
    int row_stride;
    int gimex_marker = 0;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = gimex_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return 0;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_set_marker_processor(&cinfo, JPEG_APP13, JPG_markerparser);
    gseek(ctx->stream, 0, GSEEK_SET);
    gimex_stream_src(&cinfo, ctx->stream);
    JPG_readgimexmarker = 0;
    jpeg_read_header(&cinfo, TRUE);
    gimex_marker = JPG_readgimexmarker;

    JPG_selectoutputcolourspace(&cinfo);
    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    width = cinfo.output_width;
    height = cinfo.output_height;
    channels = cinfo.output_components;

    row_buff = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    for (i = 0; i < (int)cinfo.output_height; ++i) {
        jpeg_read_scanlines(&cinfo, row_buff, 1);

        if (i < info->height) {
            JPG_readline(*row_buff, (uint8_t *)buffer, width, channels, cinfo.out_color_space, gimex_marker);
            buffer += pitch;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return retval;
}

int GIMEX_API JPG_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    struct jpeg_compress_struct cinfo;
    struct gimex_error_mgr jerr;
    J_COLOR_SPACE cspace;
    JSAMPROW row_buff = NULL;
    int bytes_per_pixel;
    int i;
    int quality;
    int width;
    int height;

    if (info->packed) {
        quality = info->quality;
    } else {
        quality = 100;
    }

    width = info->width;
    height = info->height;

    bytes_per_pixel = 3;
    cspace = JCS_RGB;

    if ((info->sub_type & 1)) {
        bytes_per_pixel = 4;
        cspace = JCS_UNKNOWN;
    } else if (info->bpp == 8) {
        bool grey_scale = true;

        for (i = 0; i < info->num_colors; ++i) {
            if (info->colortbl[i].r != info->colortbl[i].g || info->colortbl[i].r != info->colortbl[i].b) {
                grey_scale = false;
                break;
            }
        }

        if (grey_scale) {
            bytes_per_pixel = 1;
            cspace = JCS_GRAYSCALE;
        }
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = gimex_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        if (row_buff != NULL) {
            gfree(row_buff);
        }

        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_create_compress(&cinfo);
    gimex_stream_dest(&cinfo, ctx->stream);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = bytes_per_pixel;
    cinfo.in_color_space = cspace;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, true);

    if (info->dpi == 0.0) {
        cinfo.density_unit = 0;
        cinfo.X_density = 1;
        cinfo.Y_density = 1;
    } else {
        cinfo.density_unit = 1;
        cinfo.X_density = (int)info->dpi;
        cinfo.Y_density = cinfo.X_density;
    }

    jpeg_start_compress(&cinfo, true);

    if ((info->sub_type & 1) != 0) {
        jpeg_write_marker(&cinfo, JPEG_APP13, (const JOCTET *)GIMEXMARKER, sizeof(GIMEXMARKER) - 1);
    }

    row_buff = galloc(width * bytes_per_pixel);

    if (row_buff == NULL) {
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    for (i = 0; i < (int)cinfo.image_height; ++i) {
        JSAMPROW row;
        JPG_writeline((uint8_t *)buffer, row_buff, width, cspace, info);
        row = row_buff;
        jpeg_write_scanlines(&cinfo, &row, 1);
        buffer += pitch;
    }

    jpeg_finish_compress(&cinfo);
    gfree(row_buff);
    jpeg_destroy_compress(&cinfo);

    return true;
}

/*
 * Binary Match MSVC 2003 cl.exe -O1 -GF -Gm- -Oy-
 */
GABOUT *GIMEX_API JPG_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(GABOUT));
        about->signature = GIMEX_ID('J', 'P', 'E', 'G');
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
        about->requires_frame_buffer = 1;
        about->external = 0;
        about->uses_file = 1;
        about->max_frame_name = 0;
        about->default_quality = 80;
        about->mac_type[0] = GIMEX_ID('J', 'P', 'E', 'G');
        strcpy(about->extensions[0], ".jpg");
        strcpy(about->extensions[1], ".jpeg");
        strcpy(about->extensions[1], ".jfif");
        strcpy(about->author_str, "FrANK G. Barchard");
        strcpy(about->version_str, "1.12");
        strcpy(about->short_type, "JPG");
        strcpy(about->word_type, "JPeg");
        strcpy(about->long_type, "JPeg");
    }

    return about;
}

int GIMEX_API IJL_is(GSTREAM *stream)
{
    /* Since we won't be linking to the deprecated intel library, IJL funcs are mostly no-op */
    return 0;
}

int GIMEX_API IJL_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API IJL_close(GINSTANCE *ctx)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API IJL_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API IJL_wclose(GINSTANCE *ctx)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

GINFO *GIMEX_API IJL_info(GINSTANCE *ctx, int frame)
{
    GIMEX_NOTIMPLEMENTED();
    return NULL;
}

int GIMEX_API IJL_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

int GIMEX_API IJL_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    GIMEX_NOTIMPLEMENTED();
    return 0;
}

GABOUT *GIMEX_API IJL_about(void)
{
    GABOUT *about = galloc(sizeof(GABOUT));

    if (about != NULL) {
        memset(about, 0, sizeof(*about));
        about->signature = GIMEX_ID('I', 'J', 'P', 'G');
        about->size = sizeof(*about);
        about->version = 345;
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
        about->requires_frame_buffer = 1;
        about->external = 0;
        about->uses_file = 1;
        about->max_frame_name = 0;
        about->default_quality = 80;
        about->mac_type[0] = GIMEX_ID('.', 'j', 'p', 'g');
        strcpy(about->extensions[0], ".jpg");
        strcpy(about->extensions[1], ".ijpg");
        strcpy(about->extensions[1], ".ijl");
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
        strcpy(about->short_type, "ijl");
        strcpy(about->word_type, "iJPeg");
        strcpy(about->long_type, "Intel JPeg");
    }

    return about;
}
