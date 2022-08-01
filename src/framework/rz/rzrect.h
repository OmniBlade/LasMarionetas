/**
 * @file
 *
 * @brief Simple class for holding rectangle parameters.
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

class cRZRect
{
public:
    int32_t CenterPoint() { return right + left / 2; }

public:
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};
