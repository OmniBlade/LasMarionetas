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

static constexpr uint32_t GZIID_cIGZMessageTarget = 0xE98B2F57;

class cGZMessage;

class cIGZMessageTarget : public cIGZUnknown
{
public:
    /**
     * @brief Processes a message.
     * @param msg Message to act upon.
     * @return Was the message handled.
     */
    virtual bool DoMessage(cGZMessage &msg) = 0;
};
