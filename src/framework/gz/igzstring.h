/**
 * @file
 *
 * @brief An interface for string objects, similar to std::string.
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
#include <stdarg.h>

static constexpr uint32_t GZIID_cIGZString = 0x89B7DC8;

/**
 * @brief An interface for string objects, similar to std::string.
 */
class cIGZString : public cIGZUnknown
{
public:
    /**
     * @brief Populates object with string data.
     * @param source A pointer to a null terminated C string.
     */
    virtual void FromChar(const char *source) = 0;
    /**
     * @brief Populates object with string data.
     * @param source A pointer to string data.
     * @param length Number of bytes to use from the string.
     */
    virtual void FromChar(const char *source, uint32_t length) = 0;
    /**
     * @brief Fetches a pointer to the string data.
     * @return A pointer to const char data.
     */
    virtual const char *ToChar() const = 0;
    /**
     * @brief Fetches a pointer to the string data.
     * @return A pointer to const char data.
     */
    virtual const char *Data() const = 0;
    /**
     * @brief Fetches the length of the string in bytes.
     * @return Length in bytes.
     */
    virtual uint32_t Strlen() const = 0;
    /**
     * @brief Is the string empty?
     * @return Boolean indicating the state of the string.
     */
    virtual bool IsEmpty() const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param other A pointer to an const cIGZString object.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const cIGZString *other, bool case_sensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param other A reference to an const cIGZString object.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const cIGZString &other, bool case_sensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param other A pointer to a null terminated C string.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const char *other, bool case_sensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param other A pointer to string data.
     * @param length Length of the string data to use.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const char *other, uint32_t length, bool case_sensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param other A reference to an const cIGZString object.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const cIGZString &other, bool case_sensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param other A pointer to a null terminated C string.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const char *other, bool case_sensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param other A pointer to string data.
     * @param length Length of the string data to use.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const char *other, uint32_t length, bool case_sensitive) const = 0;
    /**
     * @brief Assignment operator.
     * @param other A reference to an const cIGZString object.
     * @return A reference to a cIGZString object.
     */
    virtual cIGZString &operator=(const cIGZString &other) = 0;
    /**
     * @brief Makes this string object a copy of another.
     * @param other A reference to an const cIGZString object.
     */
    virtual void Copy(const cIGZString &other) = 0;
    /**
     * @brief Resize the underlying buffer for the string.
     * @param new_size Length to be resized to.
     */
    virtual void Resize(uint32_t new_size) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param other A pointer to string data.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const char *other) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param other A pointer to string data.
     * @param length Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const char *other, uint32_t length) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param other A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const cIGZString &other) = 0;
    /**
     * @brief Inserts another string into the current one.
     * @param position Code point to insert the new string at.
     * @param other A pointer to string data.
     * @param length Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Insert(uint32_t position, const char *other, uint32_t length) = 0;
    /**
     * @brief Inserts another string into the current one.
     * @param position Code point to insert the new string at.
     * @param other A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Insert(uint32_t position, const cIGZString &other) = 0;
    /**
     * @brief Replace part of the current string with another.
     * @param position Code point to insert the new string at.
     * @param other A pointer to string data.
     * @param length Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Replace(uint32_t start_position, const char *other, uint32_t length) = 0;
    /**
     * @brief Replace part of the current string with another.
     * @param position Code point to insert the new string at.
     * @param other A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Replace(uint32_t start_position, const cIGZString &other) = 0;
    /**
     * @brief Erase part of the current string.
     * @param start_position Code point position to start erasing at.
     * @param end_position Code point position to stop erasing at.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Erase(uint32_t start_position, uint32_t end_position) = 0;
    /**
     * @brief Find the first occurance of a string within the current one.
     * @param other A pointer to string data.
     * @param position Code point position to start searching from.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t Find(const char *other, uint32_t position, bool case_sensitive) const = 0;
    /**
     * @brief Find the first occurance of a string within the current one.
     * @param other A reference to an const cIGZString object.
     * @param position Code point position to start searching from.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t Find(const cIGZString &other, uint32_t position, bool case_sensitive) const = 0;
    /**
     * @brief Find the last occurance of a string within the current one.
     * @param other A pointer to string data.
     * @param position Code point position to start searching from.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t RFind(const char *other, uint32_t position, bool case_sensitive) const = 0;
    /**
     * @brief Find the last occurance of a string within the current one.
     * @param other A reference to an const cIGZString object.
     * @param position Code point position to start searching from.
     * @param case_sensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t RFind(const cIGZString &other, uint32_t position, bool case_sensitive) const = 0;
    /**
     * @brief Populate the string with formatted content following std::sprintf conventions.
     * @param format A format string containing std::sprintf style tokens for replacement.
     * @param ... Unlimited number of arguments to replace the tokens in order.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Sprintf(const char *format, ...) = 0;
    /**
     * @brief Populate the string with formatted content following std::sprintf conventions.
     * @param format A format string containing std::sprintf style tokens for replacement.
     * @param arg_list A variadic argument list to replace the tokens with.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *SprintfVaList(const char *format, va_list arg_list) = 0;
};
