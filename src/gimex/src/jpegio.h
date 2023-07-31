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
#include "gimex.h"
#include <jpeglib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Custom input handling for libjpeg */
#define GIMEX_BUFFER_LENGTH 4096

struct gimex_source_mgr
{
    struct jpeg_source_mgr pub;
    GSTREAM *gimex_stream;
    JOCTET *gimex_buffer;
    boolean start_of_file;
};

/* Custom output handling for libjpeg */
struct gimex_destination_mgr
{
    struct jpeg_destination_mgr pub;
    GSTREAM *gimex_stream;
    JOCTET *gimex_buffer;
};

void gimex_stream_src(j_decompress_ptr cinfo, GSTREAM *stream);
void gimex_stream_dest(j_compress_ptr cinfo, GSTREAM *stream);

#ifdef __cplusplus
} // extern "C"
#endif
