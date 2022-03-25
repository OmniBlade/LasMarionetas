/**
 * @file
 *
 * @brief Interface for a container with variable contents.
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
#include "unichar.h"

static constexpr uint32_t GZIID_cIGZVariant = 0x60FD4D2B;

class cIGZString;

/**
 * This appears to have been a class in the original code, but its unclear what if anything it contained beside the enum.
 * cRZVariant inherited from both cIGZVariant and tagRZVariant but to what end?
 */
namespace tagRZVariant
{

enum Type
{
    kValUndefined,
    kValBool,
    kValUInt8,
    kValSInt8,
    kValUInt16,
    kValSInt16,
    kValUInt32,
    kValSInt32,
    kValUInt64,
    kValSInt64,
    kValFloat32,
    kValFloat64,
    kValChar,
    kValRZUnicodeChar,
    kValRZChar,
    kValVoidPtr,
    kValIGZUnknown,
};

static constexpr uint16_t kValArrayFlag = 0x8000;
static constexpr uint32_t kVariantFormatHexFlag = 1;
static constexpr uint32_t kVariantFormatBoolFlag = 2;
} // namespace tagRZVariant

class cIGZVariant : public cIGZUnknown
{
public:
    /**
     * @return Returns a value that contains type information corresponding to tagRZVariant::Type.
     */
    virtual uint16_t GetType() const = 0;
    /**
     * @return Returns the count of entries if the variant is an array.
     */
    virtual uint32_t GetCount() const = 0;
    /**
     * @return Is this variant undefined and thus has no contents.
     */
    virtual bool IsUndefined() const = 0;
    /**
     * @brief Erases the contents of the variant.
     */
    virtual void Erase() = 0;
    /**
     * @brief Copies the contents of another variant to this one.
     * @param src Variant object to copy from.
     */
    virtual void CopyFrom(const cIGZVariant &src) = 0;
    /**
     * @brief Copies the contents of this variant to another one.
     * @param dst Variant object to copy to.
     */
    virtual void CreateCopy(cIGZVariant **dst) const = 0;
    /**
     * @return Can the given type be retrieved from this variant?
     * @param type Variant type to check.
     */
    virtual bool CanGetVal(tagRZVariant::Type type) const = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValBool(bool &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual bool GetValBool() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValBool(bool val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValUint8(uint8_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual uint8_t GetValUint8() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValUint8(uint8_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValSint8(int8_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual int8_t GetValSint8() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValSint8(int8_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValUint16(uint16_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual uint16_t GetValUint16() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValUint16(uint16_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValSint16(int16_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual int16_t GetValSint16() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValSint16(int16_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValUint32(uint32_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual uint32_t GetValUint32() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValUint32(uint32_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValSint32(int32_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual int32_t GetValSint32() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValSint32(int32_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValUint64(uint64_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual uint64_t GetValUint64() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValUint64(uint64_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValSint64(int64_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual int64_t GetValSint64() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValSint64(int64_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValFloat32(float &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual float GetValFloat32() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValFloat32(float val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValFloat64(double &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual double GetValFloat64() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValFloat64(double val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValChar(char &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual char GetValChar() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValChar(char val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValRZUnicodeChar(unichar_t &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual unichar_t GetValRZUnicodeChar() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValRZUnicodeChar(unichar_t val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValRZChar(char &val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual char GetValRZChar() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValRZChar(char val) = 0;
    /**
     * @brief Get the content of this variant.
     * @param val Reference to a variable to hold the variant contents.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValVoidPtr(void *&val) const = 0;
    /**
     * @brief Get the content of this variant.
     * @return Contents of the variant.
     */
    virtual void *GetValVoidPtr() const = 0;
    /**
     * @brief Set the content of this variant.
     * @param val Value to set this variant to.
     * @return Was the value set successfully?
     */
    virtual void SetValVoidPtr(void *val) = 0;
    /**
     * @brief Create a string depicting the variant contents.
     * @param str Pointer to a cIGZString pointer to be assigned the generated string, set to null if call fails.
     * @param flags Bit flags affecting how integers and bools are represented in the string.
     * @param delimiter Character to use to delimit multiple entries in the string.
     * @return Was the string generated successfully?
     * @warning Caller is responsible for freeing pointer returned in str
     */
    virtual bool CreateValString(cIGZString **str, uint32_t flags, char delimiter) const = 0;
    /**
     * @brief Create a string depicting the variant contents.
     * @param str cIGZString reference to be hold the generated string.
     * @param flags Bit flags affecting how integers and bools are represented in the string.
     * @param delimiter Character to use to delimit multiple entries in the string.
     * @return Was the string generated successfully?
     */
    virtual bool GetValString(cIGZString &str, uint32_t flags, char delimiter) const = 0;
    /**
     * @brief Set the value as a string.
     * @param str cIGZString reference holding the string.
     */
    virtual bool SetValString(const cIGZString &str) = 0;
    /**
     * @brief Set the value from a string, interpreting as required.
     * @param str cIGZString reference holding the string.
     * @param type Variant type to convert the string to.
     * @param count How many elements does the string contain?
     * @param delimiter Character to use to delimit the elements.
     * @return Was the string generated successfully?
     */
    virtual bool SetFromString(const cIGZString &str, uint32_t type, uint32_t count, char delimiter) = 0;
    /**
     * @brief Get the value to a void type buffer.
     * @param val Pointer to memory to store the variant contents.
     * @param count Reference to an integer to hold how many elements the variant contains.
     * @return Was the string generated successfully?
     */
    virtual bool GetValVoid(void *val, uint32_t &count) const = 0;
    /**
     * @brief Set the value from a void type buffer.
     * @param val Pointer to memory buffer holding data to set the variant.
     * @param count How many elements the buffer contains.
     */
    virtual void SetValVoid(const void *val, uint32_t count) = 0;
    /**
     * @brief Get the content of this variant as a IGZUnknown derived interface.
     * @param iid Interface ID for the interface to try and retrieve.
     * @param val Pointer to a pointer to recieve the requested interface.
     * @return Was the value retrieved successfully?
     */
    virtual bool GetValUnknown(uint32_t iid, void **val) = 0;
    /**
     * @brief Get the content of this variant as a IGZUnknown interface.
     * @return Pointer to the object exposing the IGZUnknown interface.
     */
    virtual cIGZUnknown *GetValUnknown() = 0;
    /**
     * @brief Set the value from an IGZUnknown object.
     * @param val Pointer to an IGZUnknown object.
     */
    virtual void SetValUnknown(cIGZUnknown *val) = 0;
    /**
     * @brief Equality operator.
     */
    virtual bool operator==(const cIGZVariant &that) const = 0;
    /**
     * @brief Inequality operator.
     */
    virtual bool operator!=(const cIGZVariant &that) const = 0;
    /**
     * @brief Assignment operator.
     */
    virtual cIGZVariant &operator=(const cIGZVariant &that) = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual bool &AsBool() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual bool AsBool() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual uint8_t &AsUint8() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual uint8_t AsUint8() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual int8_t &AsSint8() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual int8_t AsSint8() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual uint16_t &AsUint16() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual uint16_t AsUint16() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual int16_t &AsSint16() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual int16_t AsSint16() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual uint32_t &AsUint32() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual uint32_t AsUint32() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual int32_t &AsSint32() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual int32_t AsSint32() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual uint64_t &AsUint64() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual uint64_t AsUint64() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual int64_t &AsSint64() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual int64_t AsSint64() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual float &AsFloat32() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual float AsFloat32() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual double &AsFloat64() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual double AsFloat64() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual char &AsChar() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual char AsChar() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual unichar_t &AsRZUnicodeChar() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual unichar_t AsRZUnicodeChar() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual char &AsRZChar() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual char AsRZChar() const = 0;
    /**
     * @return Modifiable reference to variant contents.
     */
    virtual void *&AsVoidPtr() = 0;
    /**
     * @return Copy of the variant contents.
     */
    virtual void *AsVoidPtr() const = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual bool *RefBool() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefBool(const bool *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual uint8_t *RefUint8() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefUint8(const uint8_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual int8_t *RefSint8() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefSint8(const int8_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual uint16_t *RefUint16() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefUint16(const uint16_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual int16_t *RefSint16() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefSint16(const int16_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual uint32_t *RefUint32() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefUint32(const uint32_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual int32_t *RefSint32() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefSint32(const int32_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual uint16_t *RefUint64() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefUint64(const uint64_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual int64_t *RefSint64() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefSint64(const int64_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual float *RefFloat32() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefFloat32(const float *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual double *RefFloat64() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefFloat64(const double *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual char *RefChar() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefChar(const char *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual unichar_t *RefRZUnicodeChar() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefRZUnicodeChar(const unichar_t *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual char *RefRZChar() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefRZChar(const char *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual void **RefVoidPtr() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefVoidPtr(const void **val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual void *RefVoid() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to memory holding data.
     * @param count How many elements of the given type to store.
     */
    virtual void RefVoid(const void *val, uint32_t count) = 0;
    /**
     * @return Pointer to variants contents.
     */
    virtual cIGZUnknown *RefIGZUnknown() const = 0;
    /**
     * @brief Sets the contents of the variant
     * @param iid Iterface ID of the object to store..
     * @param val Point to a pointer to an memory to be stored as cIGZUnknown.
     */
    virtual bool RefIGZUnknown(uint32_t iid, void **val) = 0;
    /**
     * @brief Sets the contents of the variant
     * @param val Pointer to cIGZUnknown object to store.
     */
    virtual void RefIGZUnknown(cIGZUnknown *val) = 0;
};
