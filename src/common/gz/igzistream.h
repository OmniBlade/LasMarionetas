/**
 * @file
 *
 * @brief Interface for a stream handling data input.
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

static constexpr uint32_t GZIID_cIGZIStream = 0x051B11BD;

class cIGZSerializable;
class cIGZString;
class cIGZVariant;

class cIGZIStream : public cIGZUnknown
{
    /**
     * @brief Skip data in the stream.
     * @param size The amount of data in bytes to skip.
     * @return Did we skip the requested bytes?
     */
    virtual bool Skip(uint32_t size) = 0;
    /**
     * @brief Retrieve a signed 8bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint8(int8_t &val) = 0;
    /**
     * @brief Retrieve an unsigned 8bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint8(uint8_t &val) = 0;
    /**
     * @brief Retrieve a signed 16bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint16(int16_t &val) = 0;
    /**
     * @brief Retrieve an unsigned 16bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint16(uint16_t &val) = 0;
    /**
     * @brief Retrieve a signed 32bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint32(int32_t &val) = 0;
    /**
     * @brief Retrieve an unsigned 32bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint32(uint32_t &val) = 0;
    /**
     * @brief Retrieve a signed 64bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint64(int64_t &val) = 0;
    /**
     * @brief Retrieve an unsigned 64bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint64(uint64_t &val) = 0;
    /**
     * @brief Retrieve a 32bit floating point value.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetFloat32(float &val) = 0;
    /**
     * @brief Retrieve a 64bit floating point value.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetFloat64(double &val) = 0;
    /**
     * @brief Retrieve a character string.
     * @param buf Buffer to hold the data.
     * @param size The size of the buffer allocation.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetRZCharStr(char *buf, uint32_t size) = 0;
    /**
     * @brief Retrieve a character string.
     * @param str String object to hold the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetGZStr(cIGZString &str) = 0;
    /**
     * @brief Retrieve serialized data.
     * @param serial Serializable object to hold the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetGZSerializable(cIGZSerializable &serial) = 0;
    /**
     * @brief Retrieve arbitrary data.
     * @param data Buffer to hold the data.
     * @param size The size of the buffer allocation.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetVoid(void *data, uint32_t size) = 0;
    /**
     * @return The last error value for the stream, 0 if no error.
     */
    virtual uint32_t GetError() = 0;
    /**
     * @brief Sets user data for the stream.
     * @param data Pointer to user data.
     */
    virtual void SetUserData(cIGZVariant *data) = 0;
    /**
     * @brief Gets user data for the stream.
     * @return Pointer to user data.
     */
    virtual cIGZVariant *GetUserData() = 0;
};
