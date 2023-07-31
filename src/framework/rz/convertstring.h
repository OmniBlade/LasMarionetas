/**
 * @file
 *
 * @brief String code page conversion utility functions.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <stdint.h>

class cIGZString;

int32_t ConvertStringEncoding(const void *src_string, uint32_t src_size, int32_t src_cp, void *dst_string, uint32_t &dst_size, int32_t dst_cp);
int32_t ConvertStringEncoding(const void *src_string, uint32_t src_size, int32_t src_cp, cIGZString &dst_string);
int32_t ConvertStringEncoding(const cIGZString &src_string, void *dst_string, uint32_t &dst_size, int32_t dst_cp);
bool ConvertStringEncoding(const cIGZString &src_string, int32_t src_cp, cIGZString &dst_string);
bool ConvertStringEncoding(const cIGZString &src_string, cIGZString &dst_string, int32_t dst_cp);
