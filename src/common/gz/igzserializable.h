/**
 * @file
 *
 * @brief Interface for a serializable object.
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

static constexpr uint32_t GZIID_cIGZSerializable = 0xE4FDA3D4;

class cIGZIStream;
class cIGZOStream;

class cIGZSerializable : public cIGZUnknown
{
public:
    /**
     * @brief Write object to an output stream.
     * @param ostream Output stream to write data to.
     * @return Did we encounter an error?
     */
    virtual bool Write(cIGZOStream &ostream) = 0;
    /**
     * @brief Read object from an input stream.
     * @param istream input stream to read data from.
     * @return Did we encounter an error?
     */
    virtual bool Read(cIGZIStream &istream) = 0;
    /**
     * @return Class ID of the interface implementation class.
     */
    virtual uint32_t GetGZCLSID() = 0;
};
