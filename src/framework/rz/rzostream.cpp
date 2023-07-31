/**
 * @file
 *
 * @brief Class implementing common parts of the cIGZOStream interface.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzostream.h"
#include "igzserializable.h"
#include "igzstring.h"
#include "igzvariant.h"
#include <cstring>
#include <endianness.h>

cRZOStream::cRZOStream() : mnRefCount(0), mbLittleEndian(true), mpUserData(nullptr) {}

cRZOStream::cRZOStream(bool little_endian) : mnRefCount(0), mbLittleEndian(little_endian), mpUserData(nullptr) {}

cRZOStream::~cRZOStream()
{
    if (mpUserData != nullptr) {
        mpUserData->Release();
    }
}

bool cRZOStream::QueryInterface(uint32_t iid, void **obj)
{

    switch (iid) {
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            return true;
        case GZIID_cIGZOStream:
            *obj = static_cast<cIGZOStream *>(this);
            return true;
        default:
            break;
    }

    return false;
}

uint32_t cRZOStream::AddRef()
{
    return ++mnRefCount;
}

uint32_t cRZOStream::Release()
{
    if (mnRefCount <= 1) {
        delete this;
        return 0;
    }

    return --mnRefCount;
}

bool cRZOStream::SetSint8(int8_t val)
{
    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetUint8(uint8_t val)
{
    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetSint16(int16_t val)
{
    if (mbLittleEndian) {
        val = htole16(val);
    } else {
        val = htobe16(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetUint16(uint16_t val)
{
    if (mbLittleEndian) {
        val = htole16(val);
    } else {
        val = htobe16(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetSint32(int32_t val)
{
    if (mbLittleEndian) {
        val = htole32(val);
    } else {
        val = htobe32(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetUint32(uint32_t val)
{
    if (mbLittleEndian) {
        val = htole32(val);
    } else {
        val = htobe32(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetSint64(int64_t val)
{
    if (mbLittleEndian) {
        val = htole64(val);
    } else {
        val = htobe64(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetUint64(uint64_t val)
{
    if (mbLittleEndian) {
        val = htole64(val);
    } else {
        val = htobe64(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetFloat32(float val)
{
    if (mbLittleEndian) {
        val = htolef(val);
    } else {
        val = htobef(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetFloat64(double val)
{
    if (mbLittleEndian) {
        val = htoled(val);
    } else {
        val = htobed(val);
    }

    return SetVoid(&val, sizeof(val));
}

bool cRZOStream::SetRZCharStr(const char *buf)
{
    uint32_t length = strlen(buf);
    EncodeStringLength(length);

    if (length == 0) {
        return true;
    }

    return SetVoid(buf, length);
}

bool cRZOStream::SetGZStr(const cIGZString &str)
{
    uint32_t length = str.Strlen();
    EncodeStringLength(length);

    if (length == 0) {
        return true;
    }

    return SetVoid(str.ToChar(), length);
}

bool cRZOStream::SetGZSerializable(const cIGZSerializable &serial)
{
    return serial.Write(*this);
}

void cRZOStream::SetUserData(cIGZVariant *data)
{
    mpUserData = data;
}

cIGZVariant *cRZOStream::GetUserData()
{
    return mpUserData;
}

bool cRZOStream::Init(bool little_endian)
{
    mbLittleEndian = little_endian;
    return true;
}

bool cRZOStream::Shutdown()
{
    return true;
}

void cRZOStream::EncodeStringLength(uint32_t length)
{
    if (length == 0) {
        SetUint8(0);
    } else {
        uint8_t byte;

        while (true) {
            byte = length & 0x7F;
            length >>= 7;

            if (length == 0) {
                break;
            }

            SetUint8(byte | 0x80);
        }

        SetUint8(byte & 0x7F);
    }
}
