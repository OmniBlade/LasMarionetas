/**
 * @file
 *
 * @brief Interface for exception notification generators.
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

static constexpr uint32_t GZIID_cIGZExceptionNotification = 0xA4B30150;

class cIGZExceptionNotification : public cIGZUnknown
{
public:
    /**
     * @brief Notify that an exception is about to be thrown.
     * @return If the exception should be accepted or not.
     */
    virtual bool Notify(char const *module_name, int32_t line) = 0;
};
