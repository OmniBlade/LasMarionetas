/**
 * @file
 *
 * @brief Interface for a message target.
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

static constexpr uint32_t GZIID_cIGZMessageTarget2 = 0x452294AA;

class cIGZMessage2;

class cIGZMessageTarget2 : public cIGZUnknown
{
public:
    /**
     * @brief Processes a message.
     * @param msg Message to act upon.
     * @return Was the message handled.
     */
    virtual bool DoMessage(cIGZMessage2 *msg) = 0;
};
