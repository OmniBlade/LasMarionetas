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
     * @param pszSource A pointer to a null terminated C string.
     */
    virtual void FromChar(const char *pszSource) = 0;
    /**
     * @brief Populates object with string data.
     * @param pszSource A pointer to string data.
     * @param dwLength Number of bytes to use from the string.
     */
    virtual void FromChar(const char *pszSource, uint32_t dwLength) = 0;
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
     * @param szOther A pointer to an const cIGZString object.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const cIGZString *szOther, bool bCaseSensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param szOther A reference to an const cIGZString object.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const cIGZString &szOther, bool bCaseSensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param pszOther A pointer to a null terminated C string.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const char *pszOther, bool bCaseSensitive) const = 0;
    /**
     * @brief Determines if the object contains a string equal to another.
     * @param pszOther A pointer to string data.
     * @param dwLength Length of the string data to use.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Boolean indicating if the strings equal.
     */
    virtual bool IsEqual(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param szOther A reference to an const cIGZString object.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const cIGZString &szOther, bool bCaseSensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param pszOther A pointer to a null terminated C string.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const char *pszOther, bool bCaseSensitive) const = 0;
    /**
     * @brief Compares the object string to another string.
     * @param pszOther A pointer to string data.
     * @param dwLength Length of the string data to use.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Lexographic difference between the strings.
     */
    virtual int32_t CompareTo(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const = 0;
    /**
     * @brief Assignment operator.
     * @param szOther A reference to an const cIGZString object.
     * @return A reference to a cIGZString object.
     */
    virtual cIGZString &operator=(const cIGZString &szOther) = 0;
    /**
     * @brief Makes this string object a copy of another.
     * @param szOther A reference to an const cIGZString object.
     */
    virtual void Copy(const cIGZString &szOther) = 0;
    /**
     * @brief Resize the underlying buffer for the string.
     * @param dwNewSize Length to be resized to.
     */
    virtual void Resize(uint32_t dwNewSize) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param pszOther A pointer to string data.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const char *pszOther) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param pszOther A pointer to string data.
     * @param dwLength Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const char *pszOther, uint32_t dwLength) = 0;
    /**
     * @brief Appends a string onto the end of the current one.
     * @param szOther A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Append(const cIGZString &szOther) = 0;
    /**
     * @brief Inserts another string into the current one.
     * @param dwPos Code point to insert the new string at.
     * @param pszOther A pointer to string data.
     * @param dwLength Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Insert(uint32_t dwPos, const char *pszOther, uint32_t dwLength) = 0;
    /**
     * @brief Inserts another string into the current one.
     * @param dwPos Code point to insert the new string at.
     * @param szOther A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Insert(uint32_t dwPos, const cIGZString &szOther) = 0;
    /**
     * @brief Replace part of the current string with another.
     * @param dwPos Code point to insert the new string at.
     * @param pszOther A pointer to string data.
     * @param dwLength Length of the string data to use.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Replace(uint32_t dwStartPos, const char *pszOther, uint32_t dwLength) = 0;
    /**
     * @brief Replace part of the current string with another.
     * @param dwPos Code point to insert the new string at.
     * @param szOther A reference to an const cIGZString object.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Replace(uint32_t dwStartPos, const cIGZString &szOther) = 0;
    /**
     * @brief Erase part of the current string.
     * @param dwStartPos Code point position to start erasing at.
     * @param dwEndPos Code point position to stop erasing at.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Erase(uint32_t dwStartPos, uint32_t dwEndPos) = 0;
    /**
     * @brief Find the first occurance of a string within the current one.
     * @param pszOther A pointer to string data.
     * @param dwPos Code point position to start searching from.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t Find(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const = 0;
    /**
     * @brief Find the first occurance of a string within the current one.
     * @param szOther A reference to an const cIGZString object.
     * @param dwPos Code point position to start searching from.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t Find(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const = 0;
    /**
     * @brief Find the last occurance of a string within the current one.
     * @param pszOther A pointer to string data.
     * @param dwPos Code point position to start searching from.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t RFind(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const = 0;
    /**
     * @brief Find the last occurance of a string within the current one.
     * @param szOther A reference to an const cIGZString object.
     * @param dwPos Code point position to start searching from.
     * @param bCaseSensitive Boolean to determine if comparison is case sensitive.
     * @return Position of the start of the first match.
     */
    virtual int32_t RFind(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const = 0;
    /**
     * @brief Populate the string with formatted content following std::sprintf conventions.
     * @param pszFormat A format string containing std::sprintf style tokens for replacement.
     * @param ... Unlimited number of arguments to replace the tokens in order.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *Sprintf(const char *pszFormat, ...) = 0;
    /**
     * @brief Populate the string with formatted content following std::sprintf conventions.
     * @param pszFormat A format string containing std::sprintf style tokens for replacement.
     * @param zList A variadic argument list to replace the tokens with.
     * @return Pointer to the resulting string object.
     */
    virtual cIGZString *SprintfVaList(const char *pszFormat, va_list zList) = 0;
};
