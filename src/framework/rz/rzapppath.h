/**
 * @file
 *
 * @brief Implements a function to retrieve the applications canonical path in the file system.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzstring.h"

/**
 * Retrives the path of the application binary in the file system.
 * @return Length of the retrieved string.
 */
uint32_t RZGetCurrentAppPath(cIGZString &path);
