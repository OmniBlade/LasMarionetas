/**
 * @file
 *
 * @brief Class implementing common parts of the cIGZIStream interface.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzistream.h"
#include "igzserializable.h"
#include "igzstring.h"
#include "igzvariant.h"
#include <algorithm>
#include <cstring>
#include <endianness.h>

cRZIStream::cRZIStream() : mnRefCount(0), mbLittleEndian(true), mpUserData(nullptr) {}

cRZIStream::cRZIStream(bool little_endian) : mnRefCount(0), mbLittleEndian(little_endian), mpUserData(nullptr) {}

cRZIStream::~cRZIStream()
{
    if (mpUserData != nullptr) {
        mpUserData->Release();
    }
}

bool cRZIStream::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            return true;
        case GZIID_cIGZIStream:
            *obj = static_cast<cIGZIStream *>(this);
            return true;
        default:
            break;
    }

    return false;
}

uint32_t cRZIStream::AddRef()
{
    return ++mnRefCount;
}

uint32_t cRZIStream::Release()
{
    if (mnRefCount <= 1) {
        delete this;
        return 0;
    }

    return --mnRefCount;
}

bool cRZIStream::GetSint8(int8_t &val)
{
    return GetVoid(&val, sizeof(val));
}

bool cRZIStream::GetUint8(uint8_t &val)
{
    return GetVoid(&val, sizeof(val));
}

bool cRZIStream::GetSint16(int16_t &val)
{

    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le16toh(val);
        } else {
            val = be16toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetUint16(uint16_t &val)
{

    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le16toh(val);
        } else {
            val = be16toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetSint32(int32_t &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le32toh(val);
        } else {
            val = be32toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetUint32(uint32_t &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le32toh(val);
        } else {
            val = be32toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetSint64(int64_t &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le64toh(val);
        } else {
            val = be64toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetUint64(uint64_t &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = le64toh(val);
        } else {
            val = be64toh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetFloat32(float &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = leftoh(val);
        } else {
            val = beftoh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetFloat64(double &val)
{
    if (GetVoid(&val, sizeof(val))) {
        if (mbLittleEndian) {
            val = ledtoh(val);
        } else {
            val = bedtoh(val);
        }

        return true;
    }

    return false;
}

bool cRZIStream::GetRZCharStr(char *buf, uint32_t size)
{
    char decode_buf[8];
    uint32_t decode_length;
    uint32_t buffered = DecodeStringLength(decode_length, decode_buf);

    if (GetError() != 0 || decode_length == 0) {
        buf[0] = '\0';
    } else {
        if (decode_length < size) {
            size = decode_length;
        }

        std::memcpy(buf, decode_buf, size > buffered ? buffered : size);

        if (size > buffered) {
            GetVoid(&buf[buffered], size - buffered);
        }

        buf[size] = '\0';

        if (!GetError() && decode_length != size) {
            Skip(decode_length - size);
        }
    }

    return GetError() == 0;
}

bool cRZIStream::GetGZStr(cIGZString &str)
{
    str.FromChar("");
    char decode_buf[8];
    uint32_t decode_length = 0;
    char buffer[256];
    uint32_t buffered = DecodeStringLength(decode_length, decode_buf);

    while (GetError() == 0 && decode_length > 0) {
        uint32_t chunk_size = std::min(decode_length, uint32_t(sizeof(buffer) - 1));

        if (buffered != 0) {
            std::memcpy(buffer, decode_buf, buffered < sizeof(decode_buf) ? buffered : sizeof(decode_buf));
        }

        if (decode_length > buffered) {
            GetVoid(buffer, chunk_size);
        }

        buffer[chunk_size] = '\0';
        str.Append(buffer, chunk_size);
        decode_length -= chunk_size;
        buffered = 0;
    }

    return GetError() == 0;
}

bool cRZIStream::GetGZSerializable(cIGZSerializable &serial)
{
    return serial.Read(*this);
}

void cRZIStream::SetUserData(cIGZVariant *data)
{
    mpUserData = data;
}

cIGZVariant *cRZIStream::GetUserData()
{
    return mpUserData;
}

bool cRZIStream::Init(bool little_endian)
{
    mbLittleEndian = little_endian;
    return true;
}

bool cRZIStream::Shutdown()
{
    return true;
}

uint32_t cRZIStream::DecodeStringLength(uint32_t &length, char *buffer)
{
    uint8_t bytes[8];

    int32_t current_pos = 0;
    length = 0;
    GetUint8(bytes[0]);

    if (bytes[0] != 0) {
        while ((bytes[current_pos] & 0x80) != 0) {
            bytes[current_pos++] &= 0x7F;
            GetUint8(bytes[current_pos]);
        }

        do {
            length = bytes[current_pos--] | (length << 7);
        } while (current_pos >= 0);
    }

    return 0;
}
