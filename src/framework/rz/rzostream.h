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
#pragma once
#include "igzostream.h"

class cIGZVariant;

class cRZOStream : public cIGZOStream
{
public:
    cRZOStream();
    cRZOStream(bool little_endian);
    virtual ~cRZOStream();

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
     * @brief Store a signed 8bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint8(int8_t val) override;
    /**
     * @brief Store an unsigned 8bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint8(uint8_t val) override;
    /**
     * @brief Store a signed 16bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint16(int16_t val) override;
    /**
     * @brief Store an unsigned 16bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint16(uint16_t val) override;
    /**
     * @brief Store a signed 32bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint32(int32_t val) override;
    /**
     * @brief Store an unsigned 32bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint32(uint32_t val) override;
    /**
     * @brief Store a signed 64bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetSint64(int64_t val) override;
    /**
     * @brief Store an unsigned 64bit integer.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetUint64(uint64_t val) override;
    /**
     * @brief Store a 32bit floating point value.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetFloat32(float val) override;
    /**
     * @brief Store a 64bit floating point value.
     * @param val Variable holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetFloat64(double val) override;
    /**
     * @brief Store a character string.
     * @param buf Buffer holding the null terminated data.
     * @return Did we store the provided data?
     */
    virtual bool SetRZCharStr(const char *buf) override;
    /**
     * @brief Store a character string.
     * @param str String object holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetGZStr(const cIGZString &str) override;
    /**
     * @brief Store serialized data.
     * @param serial Serializable object holding the data.
     * @return Did we store the provided data?
     */
    virtual bool SetGZSerializable(const cIGZSerializable &serial) override;
    /**
     * @brief Stores arbitrary data.
     * @param data Buffer holding the data.
     * @param size The size of the data to store.
     * @return Did we store the provided data?
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
    void EncodeStringLength(uint32_t length);

private:
    uint32_t mnRefCount;
    bool mbLittleEndian;
    cIGZVariant *mpUserData;
};
