/**
 * @file
 *
 * @brief Interface for persist db serial record objects.
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

static constexpr uint32_t GZIID_cIGZPersistDBSerialRecord = 0xC56B8F08;

class cIGZPersistDBRecord;
class cGZPersistResourceKey;
class cIGZString;
class cIGZVariant;

class cIGZPersistDBSerialRecord : public cIGZUnknown
{
public:
    /**
     * @return Implementing class as cIGZPersistDBRecord interface pointer.
     */
    virtual cIGZPersistDBRecord *AsIGZPersistDBRecord() = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint8(uint8_t &val) = 0;
    /**
     * @brinef Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint8(uint8_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint8(int8_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint8(int8_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint16(uint16_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint16(uint16_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint16(int16_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint16(int16_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint32(uint32_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint32(uint32_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint32(int32_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint32(int32_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint64(uint64_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldUint64(uint64_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint64(int64_t &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldSint64(int64_t *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldFloat32(float &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldFloat32(float *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldFloat64(double &val) = 0;
    /**
     * @brief Fetch multiple values of a type from the record.
     * @param val Pointer to appropriate memory to hold the values.
     * @param count The number of values to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldFloat64(double *val, uint32_t count) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldString(cIGZString &val) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldResKey(cGZPersistResourceKey &val) = 0;
    /**
     * @brief Fetch a single value from the record.
     * @param val Reference to an appropriate variable to hold the value.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldVariant(cIGZVariant **val) = 0;
    /**
     * @brief Fetch arbitrary data from the record.
     * @param val Pointer to appropriate memory to hold the data.
     * @param size The amount of data to retrieve.
     * @return Was the data fetched successfully?
     */
    virtual bool GetFieldVoid(void *val, uint32_t size) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint8(uint8_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint8(const uint8_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint8(int8_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint8(const int8_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint16(uint16_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint16(const uint16_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint16(int16_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint16(const int16_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint32(uint32_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint32(const uint32_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint32(int32_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint32(const int32_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint64(uint64_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldUint64(const uint64_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint64(int64_t val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldSint64(const int64_t *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldFloat32(float val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldFloat32(const float *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldFloat64(double val) = 0;
    /**
     * @brief Store multiple values of a type in the record.
     * @param val Pointer to appropriate memory holding the values.
     * @param count The number of values to store.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldFloat64(const double *val, uint32_t count) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldString(const cIGZString &val) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldResKey(const cGZPersistResourceKey &val) = 0;
    /**
     * @brief Store a single value in the record.
     * @param val Value to store in the record.
     * @return Was the value stored successfully?
     */
    virtual bool SetFieldVariant(const cIGZVariant *val) = 0;
    /**
     * @brief Store arbitrary data in the record.
     * @param val Pointer to memory holding the data.
     * @param size The amount of data to store.
     * @return Was the data stored successfully?
     */
    virtual bool SetFieldVoid(const void *val, uint32_t size) = 0;
    /**
     * @return The size of the record.
     */
    virtual int32_t GetSize() = 0;
    /**
     * @brief Set the size of the record.
     * @param size The size the record should be set to.
     * @return Was the record set to the requested size successfully?
     */
    virtual bool SetSize(uint32_t size) = 0;
    /**
     * @return The current position in the record.
     */
    virtual int32_t GetPosition() = 0;
    /**
     * @brief Seek to an absolute position in the record.
     * @param position The position in the record to seek to.
     * @return Was the seek successful?
     */
    virtual bool SeekAbsolute(uint32_t position) = 0;
    /**
     * @brief Seek to a relative position in the record.
     * @param offset The offset from the current position in the record to seek to.
     * @return Was the seek successful?
     */
    virtual bool SeekRelative(int32_t offset) = 0;
};
