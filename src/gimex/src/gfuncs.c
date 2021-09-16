/**
 * @file
 *
 * @brief Array of GIMEX codec implementation functions.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "gfuncs.h"
#include "bitmapgimex.h"
#include "jpeggimex.h"
#include "nullgimex.h"
#include "pnggimex.h"
#include "shpgimex.h"
#include "targagimex.h"
#include <stddef.h>

GimexFunctions gFunctions[] = {
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { FSH_about, FSH_is, FSH_open, FSH_info, FSH_read, FSH_close, FSH_wopen, FSH_write, FSH_wclose },
    { JPG_about, JPG_is, JPG_open, JPG_info, JPG_read, JPG_close, JPG_wopen, JPG_write, JPG_wclose },
    { IJL_about, IJL_is, IJL_open, IJL_info, IJL_read, IJL_close, IJL_wopen, IJL_write, IJL_wclose },
    { PNG_about, PNG_is, PNG_open, PNG_info, PNG_read, PNG_close, PNG_wopen, PNG_write, PNG_wclose },
    { BMP_about, BMP_is, BMP_open, BMP_info, BMP_read, BMP_close, BMP_wopen, BMP_write, BMP_wclose },
    { TGA_about, TGA_is, TGA_open, TGA_info, TGA_read, TGA_close, TGA_wopen, TGA_write, TGA_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL_about, NULL_is, NULL_open, NULL_info, NULL_read, NULL_close, NULL_wopen, NULL_write, NULL_wclose },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
};
