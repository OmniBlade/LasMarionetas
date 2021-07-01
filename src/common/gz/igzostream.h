/**
 * @file
 *
 * @brief Interface for a stream handling data output.
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

static constexpr uint32_t GZIID_cIGZOStream = 0xC51B11CB;

class cIGZSerializable;
class cIGZString;
class cIGZVariant;

class cIGZOStream : public cIGZUnknown
{
    /**
     * @brief Flush any pending data to the stream.
     */
    virtual void Flush() = 0;
    /**
     * @brief Store a signed 8bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint8(int8_t val) = 0;
    /**
     * @brief Store an unsigned 8bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint8(uint8_t val) = 0;
    /**
     * @brief Store a signed 16bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint16(int16_t val) = 0;
    /**
     * @brief Store an unsigned 16bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint16(uint16_t val) = 0;
    /**
     * @brief Store a signed 32bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint32(int32_t val) = 0;
    /**
     * @brief Store an unsigned 32bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint32(uint32_t val) = 0;
    /**
     * @brief Store a signed 64bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint64(int64_t val) = 0;
    /**
     * @brief Store an unsigned 64bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint64(uint64_t val) = 0;
    /**
     * @brief Store a 32bit floating point value.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetFloat32(float val) = 0;
    /**
     * @brief Store a 64bit floating point value.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetFloat64(double val) = 0;
    /**
     * @brief Store a character string.
     * @param buf Buffer holding the null terminated data.
     * @return Did we store the provided data?
     */
    virtual bool SetRZCharStr(const char *buf) = 0;
    /**
     * @brief Store a character string.
     * @param str String object holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetGZStr(const cIGZString &str) = 0;
    /**
     * @brief Store serialized data.
     * @param serial Serializable object holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetGZSerializable(const cIGZSerializable &serial) = 0;
    /**
     * @brief Stores arbitrary data.
     * @param data Buffer holding the data.
     * @param size The size of the data to store.
     * @return Did we store the provided data?
     */
    virtual bool SetVoid(const void *data, uint32_t size) = 0;
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
