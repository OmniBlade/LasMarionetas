/**
 * @file
 *
 * @brief Utility mainly for testing that GIMEX works as expected.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include <gimex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct GSTREAM
{
    FILE *fp;
} GSTREAM;

void *GIMEX_API galloc(uint32_t size)
{
    return malloc(size);
}

int GIMEX_API gfree(void *ptr)
{
    free(ptr);
    return true;
}

uint32_t GIMEX_API gread(GSTREAM *stream, void *dst, int32_t size)
{
    if (size < 0) {
        return 0;
    }

    uint32_t count = fread(dst, 1, size, stream->fp);

    return count;
}

uint32_t GIMEX_API gwrite(GSTREAM *stream, void *src, int32_t size)
{
    if (size < 0) {
        return 0;
    }

    uint32_t count = fwrite(src, 1, size, stream->fp);

    return count;
}

int GIMEX_API gseek(GSTREAM *stream, uint32_t pos)
{
    return fseek(stream->fp, pos, SEEK_SET) == 0;
}

int64_t GIMEX_API glen(GSTREAM *stream)
{
    size_t len = 0;
    size_t start = ftell(stream->fp);
    fseek(stream->fp, 0, SEEK_END);
    len = ftell(stream->fp);
    fseek(stream->fp, start, SEEK_SET);
    return len;
}

int main(int argc, char **argv)
{
    GSTREAM stream;
    GINFO *info;
    GINSTANCE *instance;
    GABOUT *about;
    int gimex_index;
    const char *src_name;
    const char *dst_name;
    void *buffer;

    if (argc < 3) {
        printf("USAGE: gimexconv <sourcefile> <destfile>\n");
        return -1;
    }

    src_name = argv[1];
    dst_name = argv[2];

    /* open source file and set best GIMEX module for file */
    stream.fp = fopen(src_name, "rb");

    if (stream.fp != NULL) {
        printf("Opened source file '%s' for reading.\n", src_name);

        gimex_index = 0;

        /* Quick and dirty seach by file extension, ideally search using GIMEX_is calls for best match to file sig. */
        if (strstr(src_name, ".")) {
            gimex_index = GIMEX_find(strstr(src_name, ".") + 1, 0);
        }

        if (gimex_index == 0) {
            printf("Failed to find GIMEX modules to handle '%s' for reading!\n", src_name);
            return -1;
        }

        if (GIMEX_is(&stream)) {
            about = GIMEX_about();

            if (about != NULL) {
                gimex_index = GIMEX_get();
                printf("Using GIMEX module '%s' (%d) for source file '%s'.\n", about->long_type, gimex_index, src_name);
            }

            gfree(about);

            GIMEX_open(&instance, &stream, src_name, 0);
            info = GIMEX_info(instance, 0);
            buffer = galloc(info->width * info->height * info->bpp / 8);
            GIMEX_read(instance, info, buffer, info->width * info->bpp / 8);
            GIMEX_close(instance);
            fclose(stream.fp);

            info->sub_type = 0;
            info->frame_size = 0;
            info->quality = 100;
            info->packed = 0;

            gimex_index = -1;

            if (strstr(dst_name, ".") != NULL) {
                gimex_index = GIMEX_find(strstr(dst_name, ".") + 1, 0);
            }

            if (gimex_index == -1) {
                printf("Failed to find GIMEX modules to handle '%s' for writing!\n", dst_name);
                return -1;
            }

            about = GIMEX_about();

            if (about != NULL) {
                printf("Using GIMEX module '%s' (%d) for destination file '%s'.\n", about->long_type, gimex_index, dst_name);
            }

            gfree(about);

            stream.fp = fopen(dst_name, "wb");

            if (stream.fp != NULL) {
                printf("Opened destination file '%s' for writing.\n", dst_name);
                GIMEX_wopen(&instance, &stream, dst_name, 1);
                GIMEX_write(instance, info, buffer, info->width * info->bpp / 8);
                GIMEX_wclose(instance);
                fclose(stream.fp);
            } else {
                printf("Failed to open destination file '%s' for writing!\n", dst_name);
            }

            gfree(buffer);
            gfree(info);
        } else {
            printf("GIMEX_is() failed!\n");
        }
    } else {
        printf("Failed to open source file '%s' for reading!\n", src_name);
    }

    return 0;
}
