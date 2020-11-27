/**
 * @file
 *
 * @brief An implementation of cIGZString based around std::string.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "igzstring.h"
#include "rzfastalloc.h"
#include <string>

/**
 * @brief An implementation of cIGZString based around std::string
 *
 * This class currently implements the complete virtual interface and should be abi compatable with
 * the games internal functions as well. Additional class specific functionality is partial and will
 * be implemented as required.
 */
class cRZString : public cIGZString, public std::basic_string<char, std::char_traits<char>, cRZFastSTLAllocator<char>>
{
    // std::string using a custom allocator.
    typedef std::basic_string<char, std::char_traits<char>, cRZFastSTLAllocator<char>> string_data;

public:
    cRZString();
    cRZString(const string_data &szSource);
    cRZString(const cRZString &szSource, size_t dwStart = 0, size_t dwEnd = string_data::npos);
    cRZString(const char *pszSource);
    cRZString(const char *pszSource, size_t dwLen);
    cRZString(const cIGZString &szSource);
    cRZString(char cChar, size_t dwRepetitions);

    virtual ~cRZString() {}

    virtual bool QueryInterface(uint32_t riid, void **ppvObj) override;
    virtual uint32_t AddRef() override;
    virtual uint32_t Release() override;

    virtual uint32_t FromChar(const char *pszSource) override;
    virtual uint32_t FromChar(const char *pszSource, uint32_t dwLength) override;
    virtual const char *ToChar() const override;
    virtual const char *Data() const override;

    virtual uint32_t Strlen() const override;
    virtual bool IsEmpty() const override;

    virtual bool IsEqual(const cIGZString *szOther, bool bCaseSensitive) const override;
    virtual bool IsEqual(const cIGZString &szOther, bool bCaseSensitive) const override;
    virtual bool IsEqual(const char *pszOther, bool bCaseSensitive) const override;
    virtual bool IsEqual(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const override;

    virtual int32_t CompareTo(const cIGZString &szOther, bool bCaseSensitive) const override;
    virtual int32_t CompareTo(const char *pszOther, bool bCaseSensitive) const override;
    virtual int32_t CompareTo(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const override;

    virtual cIGZString &operator=(const cIGZString &szOther) override;

    virtual int32_t Copy(const cIGZString &szOther) override;
    virtual int32_t Resize(uint32_t dwNewSize) override;

    virtual cIGZString *Append(const char *pszOther) override;
    virtual cIGZString *Append(const char *pszOther, uint32_t dwLength) override;
    virtual cIGZString *Append(const cIGZString &szOther) override;
    virtual cIGZString *Insert(uint32_t dwPos, const char *pszOther, uint32_t dwLength) override;
    virtual cIGZString *Insert(uint32_t dwPos, const cIGZString &szOther) override;
    virtual cIGZString *Replace(uint32_t dwStartPos, const char *pszOther, uint32_t dwLength) override;
    virtual cIGZString *Replace(uint32_t dwStartPos, const cIGZString &szOther) override;
    virtual cIGZString *Erase(uint32_t dwStartPos, uint32_t dwEndPos) override;

    virtual int32_t Find(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const override;
    virtual int32_t Find(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const override;
    virtual int32_t RFind(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const override;
    virtual int32_t RFind(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const override;

    virtual cIGZString *Sprintf(const char *pszFormat, ...) override;
    virtual cIGZString *SprintfVaList(const char *pszFormat, va_list zList) override;

    void MakeLower();
    void MakeUpper();

    void LTrim();
    void RTrim();
    void Trim();

    void LTrim(const char *pattern) { erase(0, find_first_not_of(pattern)); }
    void RTrim(const char *pattern) { erase(find_last_not_of(pattern) + 1); }
    void Trim(const char *pattern)
    {
        LTrim(pattern);
        RTrim(pattern);
    }

    cRZString &operator=(const cRZString &that)
    {
        if (this != &that) {
            static_cast<string_data &>(*this) = static_cast<const string_data &>(that);         
        }

        return *this;
    }

private:
    uint32_t mnRefCount;
};
