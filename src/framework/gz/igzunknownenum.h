/**
 * @file
 *
 * @brief Interface for object enumeration.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzunknown.h"

// The ID for this interface is currently unknown as it isn't used in any known released binary.
static constexpr uint32_t GZIID_cIGZUnknownEnumerator = 0x00000000;

class cIGZUnknownEnumerator : public cIGZUnknown
{
    /**
     * @brief Not much is known about the expected behaviour of this method.
     */
    virtual void Enumerate(cIGZUnknown *, cIGZUnknown *, uint32_t) = 0;
};
