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
#include <endianness.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <jpeglib.h>
#include <jerror.h>

/* Custom error handling for libjpeg */

struct gimex_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct gimex_error_mgr *g_error_ptr;

static void gimex_error_exit(j_common_ptr cinfo)
{
    g_error_ptr myerr = (g_error_ptr)cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}

/* Custom IO handling for libjpeg */
#define GIMEX_BUFFER_LENGTH 4096
struct gimex_source_mgr
{
    struct jpeg_source_mgr pub;
    GSTREAM *gimex_stream;
    JOCTET *gimex_buffer;
    bool gimex_initial;
};

struct gimex_destination_mgr
{
    struct jpeg_destination_mgr pub;
    GSTREAM *gimex_stream;
    JOCTET *gimex_buffer;
};

static void gimex_init_source(j_decompress_ptr cinfo)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;
    src->gimex_initial = true;
}

static boolean gimex_fill_input_buffer(j_decompress_ptr cinfo)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;
    int buffered = gread(src->gimex_stream, src->gimex_buffer, GIMEX_BUFFER_LENGTH);

    /* No more data */
    if (buffered == 0) {
        if (src->gimex_initial) {
            cinfo->err->msg_code = JERR_INPUT_EMPTY;
            cinfo->err->error_exit((j_common_ptr)cinfo);
        }

        cinfo->err->msg_code = JWRN_JPEG_EOF;
        cinfo->err->emit_message((j_common_ptr)cinfo, -1);
        src->gimex_buffer[0] = 0xFF;
        src->gimex_buffer[1] = 0xD9;
        buffered = 2;
    }

    src->pub.next_input_byte = src->gimex_buffer;
    src->pub.bytes_in_buffer = buffered;
    src->gimex_initial = false;

    return TRUE;
}

static void gimex_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;
    size_t remaining = num_bytes;

    while (remaining > src->pub.bytes_in_buffer) {
        remaining -= src->pub.bytes_in_buffer;
        gimex_fill_input_buffer(cinfo);
    }

    src->pub.next_input_byte += remaining;
    src->pub.bytes_in_buffer -= remaining;
}

static void gimex_term_source(j_decompress_ptr cinfo) {}

static void gimex_stream_src(j_decompress_ptr cinfo, GSTREAM* stream)
{
    struct gimex_source_mgr *src;

    if (cinfo->src == NULL) {
        src = cinfo->mem->alloc_small((j_common_ptr)cinfo, 0, sizeof(struct gimex_source_mgr));
        cinfo->src = (struct jpeg_source_mgr *)src;
        src->gimex_buffer = cinfo->mem->alloc_small((j_common_ptr)cinfo, 0, GIMEX_BUFFER_LENGTH);
    }

    src = (struct gimex_source_mgr *)cinfo->src;
    src->pub.init_source = gimex_init_source;
    src->pub.fill_input_buffer = gimex_fill_input_buffer;
    src->pub.skip_input_data = gimex_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = gimex_term_source;
    src->gimex_stream = stream;
    src->pub.bytes_in_buffer = 0;
    src->pub.next_input_byte = 0;
}

static void gimex_init_destination(j_compress_ptr cinfo)
{
    struct gimex_destination_mgr *dst = (struct gimex_destination_mgr *)cinfo->dest;
    dst->gimex_buffer = cinfo->mem->alloc_small((j_common_ptr)cinfo, 1, GIMEX_BUFFER_LENGTH);
    dst->pub.next_output_byte = dst->gimex_buffer;
    dst->pub.free_in_buffer = GIMEX_BUFFER_LENGTH;
}

static boolean gimex_empty_output_buffer(j_compress_ptr cinfo)
{
    struct gimex_destination_mgr *dst = (struct gimex_destination_mgr *)cinfo->dest;

    if (gwrite(dst->gimex_stream, dst->gimex_buffer, GIMEX_BUFFER_LENGTH) != GIMEX_BUFFER_LENGTH) {
        cinfo->err->msg_code = JERR_FILE_WRITE;
        cinfo->err->error_exit((j_common_ptr)cinfo);
    }

    dst->pub.next_output_byte = dst->gimex_buffer;
    dst->pub.free_in_buffer = GIMEX_BUFFER_LENGTH;

    return true;
}

static void gimex_term_destination(j_compress_ptr cinfo)
{
    struct gimex_destination_mgr *dst = (struct gimex_destination_mgr *)cinfo->dest;
    int remaining;

    if (dst->pub.free_in_buffer != GIMEX_BUFFER_LENGTH) {
        remaining = GIMEX_BUFFER_LENGTH - dst->pub.free_in_buffer;
        if (gwrite(dst->gimex_stream, dst->gimex_buffer, remaining) != remaining) {
            cinfo->err->msg_code = JERR_FILE_WRITE;
            cinfo->err->error_exit((j_common_ptr)cinfo);
        }
    }
}

static void gimex_stream_dest(j_compress_ptr cinfo, GSTREAM* stream) {
    struct gimex_destination_mgr *dst;

    if (cinfo->dest == NULL) {
        cinfo->dest = cinfo->mem->alloc_small((j_common_ptr)cinfo, 0, sizeof(struct gimex_destination_mgr));
    }
    
    dst = (struct gimex_destination_mgr *)cinfo->dest;
    dst->pub.init_destination = gimex_init_destination;
    dst->pub.empty_output_buffer = gimex_empty_output_buffer;
    dst->pub.term_destination = gimex_term_destination;
    dst->gimex_stream = stream;
}

/* Custom marker handler for gimex written JPEG files */

/* Define the marker GIMEX will use in the jpeg standard */
#define JPEG_APP13 0xED

static int JPG_readgimexmarker;
static const char MARKER_CONST[] = "GIMEXARGB";

static boolean JPG_markerparser(j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr *src = cinfo->src;
    const JOCTET *byte = src->next_input_byte;
    size_t bytes_in_buff = src->bytes_in_buffer;
    char read_marker[10];
    int marker_length;
    int marker_remaining;

    if (bytes_in_buff == 0) {
        if (!src->fill_input_buffer(cinfo)) {
            return 0;
        }

        byte = src->next_input_byte;
        bytes_in_buff = src->bytes_in_buffer;
    }

    marker_length = *byte++ << 8;
    --bytes_in_buff;

    if (bytes_in_buff == 0) {
        if (!src->fill_input_buffer(cinfo)) {
            return 0;
        }

        byte = src->next_input_byte;
        bytes_in_buff = src->bytes_in_buffer;
    }

    marker_length += *byte++;
    --bytes_in_buff;
    marker_remaining = marker_length - 2;

    if (marker_remaining >= sizeof(MARKER_CONST) - 1) {
        for (int i = 0; i < sizeof(read_marker) - 1; ++i) {
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

        marker_remaining = marker_length - 11;
    }

    if (memcmp(read_marker, MARKER_CONST, sizeof(MARKER_CONST) - 1) == 0) {
        JPG_readgimexmarker = 1;
    }

    src->next_input_byte = byte;
    src->bytes_in_buffer = bytes_in_buff;

    if (marker_remaining > 0) {
        cinfo->src->skip_input_data(cinfo, marker_remaining);
    }

    return 1;
}

/* Helper functions for parsing the JPEG file */
void JPG_readcmyk(unsigned char *dst, const unsigned char *src)
{
    int c, m, y, k;

    k = src[3];
    c = src[0] + k < 255 ? src[0] + k : -1;
    m = src[1] + k < 255 ? src[1] + k : -1;
    y = src[2] + k < 255 ? src[2] + k : -1;

    dst[0] = -1 - c;
    dst[0] = -1 - m;
    dst[0] = -1 - y;
}

void JPG_readline(char *dst, const char *src, int width, J_COLOR_SPACE cspace, bool gimex_format)
{
    char *put_ptr = dst;
    const char *get_ptr = src;

    if (gimex_format) {
        for (int i = 0; i < width; ++i) {
            put_ptr[3] = get_ptr[0];
            put_ptr[2] = get_ptr[1];
            put_ptr[1] = get_ptr[2];
            put_ptr[0] = get_ptr[3];

            put_ptr += 4;
            get_ptr += 4;
        }
    } else if (cspace == JCS_GRAYSCALE) {
        memcpy(dst, src, width);
    } else if (cspace == JCS_CMYK) {
        for (int i = 0; i < width; ++i) {
            char tmp[3];
            JPG_readcmyk(tmp, get_ptr);
            put_ptr[3] = -1;
            put_ptr[2] = tmp[0];
            put_ptr[1] = tmp[1];
            put_ptr[0] = tmp[2];

            put_ptr += 4;
            get_ptr += 4;
        }
    } else {
        for (int i = 0; i < width; ++i) {
            put_ptr[3] = -1;
            put_ptr[2] = get_ptr[0];
            put_ptr[1] = get_ptr[1];
            put_ptr[0] = get_ptr[2];

            put_ptr += 4;
            get_ptr += 3;
        }
    }
}

void JPG_writeline(const char* src, char* dst, int width, J_COLOR_SPACE cspace, const GINFO *info)
{
    switch (cspace) {
        case JCS_GRAYSCALE:
            if (info->bpp == 32) {
                for (int i = 0; i < width; ++i) {
                    dst[i] = ((ARGB *)src)[i].g;
                }
            } else {
                for (int i = 0; i < width; ++i) {
                    dst[i] = info->colortbl[src[i]].g;
                }
            }
            break;
        case JCS_RGB:
            if (info->bpp == 32) {
                for (int i = 0; i < width; ++i) {
                    dst[3 * i] = ((ARGB *)src)[i].r;
                    dst[3 * i + 1] = ((ARGB *)src)[i].g;
                    dst[3 * i + 2] = ((ARGB *)src)[i].b;
                }
            } else {
                for (int i = 0; i < width; ++i) {
                    dst[3 * i] = info->colortbl[src[i]].r;
                    dst[3 * i + 1] = info->colortbl[src[i]].g;
                    dst[3 * i + 2] = info->colortbl[src[i]].b;
                }
            }
            break;
        case JCS_UNKNOWN:
            if (info->bpp == 32) {
                for (int i = 0; i < width; ++i) {
                    dst[4 * i] = ((ARGB *)src)[i].a;
                    dst[4 * i + 1] = ((ARGB *)src)[i].r;
                    dst[4 * i + 2] = ((ARGB *)src)[i].g;
                    dst[4 * i + 3] = ((ARGB *)src)[i].b;
                }
            } else {
                for (int i = 0; i < width; ++i) {
                    dst[4 * i] = info->colortbl[src[i]].a;
                    dst[4 * i + 1] = info->colortbl[src[i]].r;
                    dst[4 * i + 2] = info->colortbl[src[i]].g;
                    dst[4 * i + 3] = info->colortbl[src[i]].b;
                }
            }
            break;
    }
}

/* GIMEX interface implementations */

int GIMEX_API JPG_is(GSTREAM *stream)
{
    uint32_t file_id;

    gseek(stream, 0);
    
    if (gread(stream, &file_id, sizeof(file_id)) != sizeof(file_id)) {
        return 0;
    }

    file_id = be32toh(file_id);

    if (file_id == 0xFFD8FFE0 || file_id == 0xFFD8FFE1 || file_id == 0xFFD8FFED) {
        return 100;
    }

    return 0;
}

int GIMEX_API JPG_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2)
{
    GINSTANCE *inst = galloc(sizeof(GINSTANCE));

    if (inst == NULL) {
        return 0;
    }

    memset(inst, 0, sizeof(GINSTANCE));
    inst->frames = 1;
    inst->signature = GIMEX_ID('J', 'P', 'E', 'G');
    inst->size = sizeof(GINSTANCE);
    inst->stream = stream;
    *ctx = inst;

    return 1;
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
    gseek(ctx->stream, 0);
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

            for (int i = 0; i != 256; ++i) {
                info->colortbl[i].a = 255;
                info->colortbl[i].r = (unsigned char)i;
                info->colortbl[i].g = (unsigned char)i;
                info->colortbl[i].b = (unsigned char)i;
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
    struct jpeg_decompress_struct cinfo;
    struct gimex_error_mgr jerr;
    JSAMPARRAY row_buff;
    bool gimex_marker;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = gimex_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_set_marker_processor(&cinfo, JPEG_APP13, JPG_markerparser);
    gseek(ctx->stream, 0);
    gimex_stream_src(&cinfo, ctx->stream);
    JPG_readgimexmarker = false;
    jpeg_read_header(&cinfo, TRUE);
    gimex_marker = JPG_readgimexmarker;

    if (cinfo.jpeg_color_space == JCS_YCbCr) {
        cinfo.out_color_space = JCS_RGB;
    } else if (cinfo.jpeg_color_space == JCS_YCCK) {
        cinfo.out_color_space = JCS_CMYK;
    }

    jpeg_start_decompress(&cinfo);
    row_buff = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, 1, cinfo.output_components * cinfo.output_width, 1);

    for (int i = 0; i < (int)cinfo.output_height; ++i) {
        jpeg_read_scanlines(&cinfo, row_buff, 1);

        if (i < info->height) {
            JPG_readline(buffer, *row_buff, cinfo.output_width, cinfo.out_color_space, gimex_marker);
            buffer += pitch;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return true;
}

int GIMEX_API JPG_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch)
{
    struct jpeg_compress_struct cinfo;
    struct gimex_error_mgr jerr;
    J_COLOR_SPACE cspace;
    JSAMPROW row_buff = NULL;
    int bytes_per_pixel;
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

        for (int i = 0; i < info->num_colors; ++i) {
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
        jpeg_write_marker(&cinfo, JPEG_APP13, MARKER_CONST, sizeof(MARKER_CONST) - 1);
    }

    row_buff = galloc(width * bytes_per_pixel);

    if (row_buff == NULL) {
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    for (unsigned i = 0; i < cinfo.image_height; ++i) {
        JSAMPROW row;
        JPG_writeline(buffer, row_buff, width, cspace, info);
        row = row_buff;
        jpeg_write_scanlines(&cinfo, &row, 1);
        buffer += pitch;
    }

    jpeg_finish_compress(&cinfo);
    gfree(row_buff);
    jpeg_destroy_compress(&cinfo);

    return true;
}

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
        strcpy(about->author_str, "Assembly Armada");
        strcpy(about->version_str, "1.00");
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
