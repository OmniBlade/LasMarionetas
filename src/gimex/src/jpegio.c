/**
 * @file
 *
 * @brief Custom IO functions for libjpeg.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include <jpegio.h>
#include <jerror.h>

void gimex_init_source(j_decompress_ptr cinfo)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;
    src->start_of_file = TRUE;
}

boolean gimex_fill_input_buffer(j_decompress_ptr cinfo)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;
    size_t buffered;

    buffered = gread(src->gimex_stream, src->gimex_buffer, GIMEX_BUFFER_LENGTH);

    /* No more data */
    if (buffered <= 0) {
        if (src->start_of_file) {
            cinfo->err->msg_code = JERR_INPUT_EMPTY;
            cinfo->err->error_exit((j_common_ptr)cinfo);
        }

        cinfo->err->msg_code = JWRN_JPEG_EOF;
        cinfo->err->emit_message((j_common_ptr)cinfo, -1);
        src->gimex_buffer[0] = (JOCTET)0xFF;
        src->gimex_buffer[1] = (JOCTET)JPEG_EOI;
        buffered = 2;
    }

    src->pub.next_input_byte = src->gimex_buffer;
    src->pub.bytes_in_buffer = buffered;
    src->start_of_file = FALSE;

    return TRUE;
}

void gimex_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    struct gimex_source_mgr *src = (struct gimex_source_mgr *)cinfo->src;

    if (num_bytes > 0) {
        while (num_bytes > (long)src->pub.bytes_in_buffer) {
            num_bytes -= (long)src->pub.bytes_in_buffer;
            gimex_fill_input_buffer(cinfo);
        }

        src->pub.next_input_byte += (size_t)num_bytes;
        src->pub.bytes_in_buffer -= (size_t)num_bytes;
    }
}

void gimex_term_source(j_decompress_ptr cinfo)
{
    /* So much empty. */
}

void gimex_stream_src(j_decompress_ptr cinfo, GSTREAM *stream)
{
    struct gimex_source_mgr *src;

    if (cinfo->src == NULL) {
        cinfo->src =
            (struct jpeg_source_mgr *)cinfo->mem->alloc_small((j_common_ptr)cinfo, 0, sizeof(struct gimex_source_mgr));
        src = (struct gimex_source_mgr *)cinfo->src;
        src->gimex_buffer = (JOCTET *)cinfo->mem->alloc_small((j_common_ptr)cinfo, 0, GIMEX_BUFFER_LENGTH);
    }

    src = (struct gimex_source_mgr *)cinfo->src;
    src->pub.init_source = gimex_init_source;
    src->pub.fill_input_buffer = gimex_fill_input_buffer;
    src->pub.skip_input_data = gimex_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = gimex_term_source;
    src->gimex_stream = stream;
    src->pub.bytes_in_buffer = 0;
    src->pub.next_input_byte = NULL;
}

void gimex_init_destination(j_compress_ptr cinfo)
{
    struct gimex_destination_mgr *dst = (struct gimex_destination_mgr *)cinfo->dest;
    dst->gimex_buffer = cinfo->mem->alloc_small((j_common_ptr)cinfo, 1, GIMEX_BUFFER_LENGTH);
    dst->pub.next_output_byte = dst->gimex_buffer;
    dst->pub.free_in_buffer = GIMEX_BUFFER_LENGTH;
}

boolean gimex_empty_output_buffer(j_compress_ptr cinfo)
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

void gimex_term_destination(j_compress_ptr cinfo)
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

void gimex_stream_dest(j_compress_ptr cinfo, GSTREAM *stream)
{
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
