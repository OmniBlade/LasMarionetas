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
#pragma once
#include "igzistream.h"

class cIGZVariant;

class cRZIStream : public cIGZIStream
{
public:
    cRZIStream();
    cRZIStream(bool little_endian);
    virtual ~cRZIStream();

    /**
     * @brief Casts the object to the interface specified by riid
     * @param riid The GUID for the interface type to cast to
     * @param obj A pointer to a void pointer to store the result in
     * @return Whether the object could be successfully cast or not
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;
    /**
     * @brief Adds a reference to this object
     * @return The new reference count for this object
     */
    virtual uint32_t AddRef() override;
    /**
     * @brief Removes a reference to this object
     * @return The new reference count for this object
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;

    /**
     * @brief Retrieve a signed 8bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint8(int8_t &val) override;
    /**
     * @brief Retrieve an unsigned 8bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint8(uint8_t &val) override;
    /**
     * @brief Retrieve a signed 16bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint16(int16_t &val) override;
    /**
     * @brief Retrieve an unsigned 16bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint16(uint16_t &val) override;
    /**
     * @brief Retrieve a signed 32bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint32(int32_t &val) override;
    /**
     * @brief Retrieve an unsigned 32bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint32(uint32_t &val) override;
    /**
     * @brief Retrieve a signed 64bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetSint64(int64_t &val) override;
    /**
     * @brief Retrieve an unsigned 64bit integer.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetUint64(uint64_t &val) override;
    /**
     * @brief Retrieve a 32bit floating point value.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetFloat32(float &val) override;
    /**
     * @brief Retrieve a 64bit floating point value.
     * @param val Variable reference to receive the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetFloat64(double &val) override;
    /**
     * @brief Retrieve a character string.
     * @param buf Buffer to hold the data.
     * @param size The size of the buffer allocation.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetRZCharStr(char *buf, uint32_t size) override;
    /**
     * @brief Retrieve a character string.
     * @param str String object to hold the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetGZStr(cIGZString &str) override;
    /**
     * @brief Retrieve serialized data.
     * @param serial Serializable object to hold the data.
     * @return Did we retrieve the requested data?
     */
    virtual bool GetGZSerializable(cIGZSerializable &serial) override;
    /**
     * @brief Sets user data for the stream.
     * @param data Pointer to user data.
     */
    virtual void SetUserData(cIGZVariant *data) override;
    /**
     * @brief Gets user data for the stream.
     * @return Pointer to user data.
     */
    virtual cIGZVariant *GetUserData() override;
    /**
     * @brief Initialize the stream.
     * @param little_endian Should thhe data in the stream be treated as little endian data?
     * @return Was the stream initialised successfully.
     */
    virtual bool Init(bool little_endian);
    /**
     * @brief Shut down the stream.
     * @return Was the stream shut down successfully.
     */
    virtual bool Shutdown();

private:
    /**
     * Use of this function suggests it originally read some of the string as well, but now only retrieves the length.
     */
    uint32_t DecodeStringLength(uint32_t &length, char *buffer);

private:
    uint32_t mnRefCount;
    bool mbLittleEndian;
    cIGZVariant *mpUserData;
};
