/**
 * @file
 *
 * @brief Gimex public header.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <endianness.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

/* Detect when a call is made to a function we didn't implement yet. */
#include <stdio.h>
#define GIMEX_NOTIMPLEMENTED() \
    do { \
        printf("%s, %d, Called '%s' which is not implemented\n", __FILE__, __LINE__, __CURRENT_FUNCTION__); \
    } while (0)

/* Original binary uses __stdcall so need to emulate that for msvc 32bit builds */
#if defined _MSC_VER && defined _M_IX86
#define GIMEX_API __stdcall
#else
#define GIMEX_API
#endif

#define GIMEX_VERSION 345

#define GIMEX_FRAMENAME_SIZE 512
#define GIMEX_COMMENT_SIZE 1024
#define GIMEX_COLOURTBL_SIZE 256
#define GIMEX_HOTSPOTTBL_SIZE 1024
#define GIMEX_HOTSPOTTBL_VALUES 2

#define GIMEX_MACTYPES 8
#define GIMEX_EXTENSIONS 8
#define GIMEX_EXTENSION_SIZE 8
#define GIMEX_AUTHORSTR_SIZE 32
#define GIMEX_VERSIONSTR_SIZE 8
#define GIMEX_SHORTTYPESTR_SIZE 8
#define GIMEX_WORDTYPESTR_SIZE 16
#define GIMEX_LONGTYPESTR_SIZE 32
#define GIMEX_ID(a, b, c, d) ((((int)(a)) << 24) | (((int)(b)) << 16) | (((int)(c)) << 8) | (int)(d))

#define GSEEK_SET 0

#ifndef gmin
#define gmin(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef gmax
#define gmax(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Opaque file handle, exact definition defined by application */
typedef struct GSTREAM GSTREAM;

typedef unsigned char GCHANNEL;

#if defined __LITTLE_ENDIAN__
typedef struct
{
    GCHANNEL b, g, r, a;
} ARGB;
#elif defined __BIG_ENDIAN__
typedef struct
{
    GCHANNEL a, r, g, b;
} ARGB;
#endif

typedef struct GABOUT
{
    int signature;
    int size;
    int version;
    unsigned can_import : 1;
    unsigned can_export : 1;
    unsigned import_packed : 2;
    unsigned export_packed : 2;
    unsigned import_paletted : 1;
    unsigned export_paletted : 1;
    unsigned import_argb : 1;
    unsigned export_argb : 1;
    unsigned multi_frame : 1;
    unsigned multi_file : 1;
    unsigned multi_size : 1;
    unsigned requires_frame_buffer : 1;
    unsigned external : 1;
    unsigned uses_file : 1;
    unsigned single_palette : 1;
    unsigned greyscale : 1;
    unsigned start_colour : 1;
    unsigned dotsubtype : 1;
    unsigned resizable : 1;
    unsigned unused1 : 1;
    unsigned unused2 : 1;
    unsigned import_stream : 1;
    unsigned export_stream : 1;
    unsigned movie : 1;
    unsigned mip_maps : 1;
    unsigned font : 1;
    unsigned obsolete : 1;
    unsigned large_files : 1;
    unsigned first_extension : 1;
    unsigned unused3 : 1;
    int max_colors;
    int max_frame_name;
    int default_quality;
    int mac_type[GIMEX_MACTYPES];
    char extensions[GIMEX_EXTENSIONS][GIMEX_EXTENSION_SIZE];
    char author_str[GIMEX_AUTHORSTR_SIZE];
    char version_str[GIMEX_VERSIONSTR_SIZE];
    char short_type[GIMEX_SHORTTYPESTR_SIZE];
    char word_type[GIMEX_WORDTYPESTR_SIZE];
    char long_type[GIMEX_LONGTYPESTR_SIZE];
    unsigned char max_alpha_bits;
    unsigned char max_red_bits;
    unsigned char max_green_bits;
    unsigned char max_blue_bits;
    unsigned max_width;
    unsigned max_height;
    unsigned align_width;
    unsigned align_height;
    unsigned unused[4];
} GABOUT;

typedef struct GINSTANCE
{
    int signature;
    int size;
    int frames;
    int frame_num;
    GSTREAM *stream;
    void *image_context;
} GINSTANCE;

typedef struct GINFO
{
    int signature;
    int size;
    int version;
    int frame_num;
    int width;
    int height;
    int bpp;
    int original_bpp;
    int palette_start_index;
    int num_colors;
    ARGB colortbl[GIMEX_COLOURTBL_SIZE];
    int sub_type;
    int packed;
    int quality;
    int frame_size;
    int alpha_bits;
    int red_bits;
    int green_bits;
    int blue_bits;
    int center_x;
    int center_y;
    int default_x;
    int default_y;
    int num_hotspots;
    char frame_name[GIMEX_FRAMENAME_SIZE];
    char comment[GIMEX_COMMENT_SIZE];
    int hotspot_tbl[GIMEX_HOTSPOTTBL_SIZE][GIMEX_HOTSPOTTBL_VALUES];
    float dpi;
    float fps;
    int unused[3];
} GINFO;

/*** "Standard" GIMEX libary functions. These should be defined by the application implementing GIMEX ***/

/**
 * @brief Allocate memory on the heap for use by GIMEX functions.
 * @param size Size of the allocation in bytes.
 * @return Pointer to the memory allocated, null if the allocation failed.
 */
void *GIMEX_API galloc(uint32_t size);
/**
 * @brief Free memory allocated by the galloc GIMEX function.
 * @param ptr Pointer to the memory block to free.
 * @return Was the memory freed successfully, 0 if unsuccessful.
 */
int GIMEX_API gfree(void *ptr);
/**
 * @brief Reads data from a stream.
 * @param stream Stream to read from.
 * @param dst Pointer to memory to store the read data.
 * @param size Amount of data in bytes to read.
 * @return Amount of data successfully transferred to the destination.
 */
uint32_t GIMEX_API gread(GSTREAM *stream, void *dst, int32_t size);
/**
 * @brief Writes data to a stream.
 * @param stream Stream to write to.
 * @param src Pointer to memory to write data from.
 * @param size Amount of data in bytes to write.
 * @return Amount of data successfully written to the stream.
 */
uint32_t GIMEX_API gwrite(GSTREAM *stream, void *src, int32_t size);
/**
 * @brief Seeks to a specific position in the stream.
 * @param stream Stream to seek.
 * @param pos Position in the stream to seek to.
 * @return Was the seek successful? 0 if not.
 */
int GIMEX_API gseek(GSTREAM *stream, int32_t pos, int32_t whence);
/**
 * @brief Get the length of a stream.
 * @param stream Stream to query.
 * @return Length of the stream.
 */
int64_t GIMEX_API glen(GSTREAM *stream);

/*** GIMEX functions for using graphical data, use these in your application to handle all supported GIMEX formats. ***/

/**
 * @brief Get probability the stream contains a file of the current GIMEX file format.
 * @param stream Stream to query.
 * @return Probability the file is the currently set file type.
 */
int GIMEX_API GIMEX_is(GSTREAM *stream);
/**
 * @brief Open a GIMEX file context from a stream.
 * @param ctx Pointer to a GimexInstance pointer to recieve the allocated context.
 * @param stream Stream to open from.
 * @return Was the context opened successfully.
 */
int GIMEX_API GIMEX_open(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
/**
 * @brief Close a GIMEX file context.
 * @param ctx Pointer to a GimexInstance to close.
 * @return Was the context closed successfully.
 */
int GIMEX_API GIMEX_close(GINSTANCE *ctx);
/**
 * @brief Open a GIMEX file context from a stream.
 * @param ctx Pointer to a GimexInstance pointer to recieve the allocated context.
 * @param stream Stream to open from.
 * @return Was the context opened successfully.
 */
int GIMEX_API GIMEX_wopen(GINSTANCE **ctx, GSTREAM *stream, const char *unk1, bool unk2);
/**
 * @brief Close a GIMEX file context.
 * @param ctx Pointer to a GimexInstance to close.
 * @return Was the context closed successfully.
 */
int GIMEX_API GIMEX_wclose(GINSTANCE *ctx);
/**
 * @brief Retrieves information about the graphics file.
 * @param ctx Pointer to a GimexInstance context.
 * @param frame Frame to get info on in multi frame formats.
 * @return Pointer to an GINFO struct. Caller must clean up with gfree.
 */
GINFO *GIMEX_API GIMEX_info(GINSTANCE *ctx, int frame);
/**
 * @brief Reads graphical data from a file.
 * @param ctx Pointer to a GimexInstance context.
 * @param info Pointer to a GINFO struct.
 * @param buffer Pointer to a buffer to store the image data.
 * @param pitch Size of a row in the image buffer.
 * @return Was the data read successfully.
 */
bool GIMEX_API GIMEX_read(GINSTANCE *ctx, GINFO *info, char *buffer, int pitch);
/**
 * @brief Writes graphical data to a file.
 * @param ctx Pointer to a GimexInstance context.
 * @param info Pointer to a GINFO struct.
 * @param buffer Pointer to a buffer storing the image data.
 * @param pitch Size of a row in the image buffer.
 * @return Was the data written successfully.
 */
bool GIMEX_API GIMEX_write(GINSTANCE *ctx, const GINFO *info, char *buffer, int pitch);
/**
 * @brief Retrieves information about the current GIMEX codec.
 * @return GABOUT struct filled with the infomation. Caller must free with gfree.
 */
GABOUT *GIMEX_API GIMEX_about(void);
/**
 * @brief Sets the currently active codec index.
 * @param codec Index of the desired codec.
 */
void GIMEX_API GIMEX_set(int codec);
/**
 * @brief Retrieves the currently active codec index.
 * @return Current GIMEX codex index.
 */
int GIMEX_API GIMEX_get(void);
/**
 * @brief Find a suitable codec for the provided format.
 * @param formats String containing extension of the desired format.
 * @param format_index Pointer to an int to recieve sub format index for codec.
 * @return Located GIMEX codex index.
 */
int GIMEX_API GIMEX_find(const char *formats, int *format_index);

#ifdef __cplusplus
} // extern "C"
#endif
