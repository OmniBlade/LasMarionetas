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
#include "rzstring.h"
#include <algorithm>
#include <cstring>
#include <strings.h>

#ifdef DLL_BUILD
// Ensure object is correct size for ABI compatibility.
static_assert(sizeof(cRZString) == 0x14, "cRZString is not the expected size for ABI compat");
#endif

cRZString::cRZString() : mnRefCount(0) {}

cRZString::cRZString(const string_data &szSource) : string_data(szSource), mnRefCount(0) {}

cRZString::cRZString(const cRZString &szSource, size_t dwStart, size_t dwEnd) :
    string_data(szSource, dwStart, dwEnd), mnRefCount(0)
{
}

cRZString::cRZString(const char *pszSource) : string_data(pszSource), mnRefCount(0) {}

cRZString::cRZString(const char *pszSource, size_t dwLen) : string_data(pszSource, dwLen), mnRefCount(0) {}

cRZString::cRZString(const cIGZString &szSource) : string_data(szSource.Data(), szSource.Strlen()), mnRefCount(0) {}

cRZString::cRZString(char cChar, size_t dwRepetitions) : string_data(dwRepetitions, cChar), mnRefCount(0) {}

bool cRZString::QueryInterface(uint32_t riid, void **ppvObj)
{
    switch (riid) {
        case GZIID_cIGZUnknown:
            *ppvObj = static_cast<cIGZUnknown *>(this);
            AddRef();
            return true;
        case GZIID_cIGZString:
            *ppvObj = static_cast<cIGZString *>(this);
            AddRef();
            return true;
        default:
            break;
    }
    return false;
}

uint32_t cRZString::AddRef()
{
    return ++mnRefCount;
}

uint32_t cRZString::Release()
{
    --mnRefCount;

    if (mnRefCount <= 0) {
        delete this;
        return 0;
    }

    return mnRefCount;
}

void cRZString::FromChar(const char *pszSource)
{
    if (pszSource == nullptr) {
        erase();
    } else {
        assign(pszSource);
    }
}

void cRZString::FromChar(const char *pszSource, uint32_t dwLength)
{
    if (pszSource == nullptr) {
        erase();
    } else {
        assign(pszSource, dwLength);
    }
}

const char *cRZString::ToChar() const
{
    return c_str();
}

const char *cRZString::Data() const
{
    return c_str();
}

uint32_t cRZString::Strlen() const
{
    return (uint32_t)length();
}

bool cRZString::IsEmpty() const
{
    return empty();
}

bool cRZString::IsEqual(const cIGZString *szOther, bool bCaseSensitive) const
{
    if (szOther == nullptr) {
        return Strlen() == 0;
    }

    cRZString tmp(szOther->ToChar());

    return CompareTo(tmp, bCaseSensitive) == 0;
}

bool cRZString::IsEqual(const cIGZString &szOther, bool bCaseSensitive) const
{
    return CompareTo(szOther, bCaseSensitive) == 0;
}

bool cRZString::IsEqual(const char *pszOther, bool bCaseSensitive) const
{
    if (pszOther == nullptr) {
        return Strlen() == 0;
    }

    return CompareTo(pszOther, strlen(pszOther), bCaseSensitive) == 0;
}

bool cRZString::IsEqual(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const
{
    if (pszOther == nullptr) {
        return Strlen() == 0;
    }

    return CompareTo(pszOther, dwLength, bCaseSensitive) == 0;
}

int32_t cRZString::CompareTo(const cIGZString &szOther, bool bCaseSensitive) const
{
    uint32_t other_len = szOther.Strlen();
    uint32_t cmp_len = std::min((uint32_t)size(), other_len);

    int32_t ret;

    if (bCaseSensitive) {
        ret = std::strncmp(c_str(), szOther.ToChar(), cmp_len);
    } else {
        ret = strncasecmp(c_str(), szOther.ToChar(), cmp_len);
    }

    if (ret == 0) {
        ret = size() - other_len;
    }

    return ret;
}

int32_t cRZString::CompareTo(const char *pszOther, bool bCaseSensitive) const
{
    return CompareTo(pszOther, strlen(pszOther), bCaseSensitive);
}

int32_t cRZString::CompareTo(const char *pszOther, uint32_t dwLength, bool bCaseSensitive) const
{
    if (dwLength == -1) {
        dwLength = strlen(pszOther);
    }

    uint32_t cmp_len = std::min((uint32_t)size(), dwLength);
    int32_t ret;

    if (bCaseSensitive) {
        ret = std::strncmp(c_str(), pszOther, cmp_len);
    } else {
        ret = strncasecmp(c_str(), pszOther, cmp_len);
    }

    if (ret == 0) {
        ret = size() - dwLength;
    }

    return ret;
}

cIGZString &cRZString::operator=(const cIGZString &szOther)
{
    Copy(szOther);

    return *this;
}

void cRZString::Copy(const cIGZString &szOther)
{
    FromChar(szOther.Data(), szOther.Strlen());
}

void cRZString::Resize(uint32_t dwNewSize)
{
    resize(dwNewSize);
}

cIGZString *cRZString::Append(const char *pszOther)
{
    if (pszOther != nullptr) {
        append(pszOther, strlen(pszOther));
    }

    return this;
}

cIGZString *cRZString::Append(const char *pszOther, uint32_t dwLength)
{
    append(pszOther, dwLength);

    return this;
}

cIGZString *cRZString::Append(const cIGZString &szOther)
{
    append(szOther.ToChar(), szOther.Strlen());

    return this;
}

cIGZString *cRZString::Insert(uint32_t dwPos, const char *pszOther, uint32_t dwLength)
{
    insert(dwPos, pszOther, dwLength);

    return this;
}

cIGZString *cRZString::Insert(uint32_t dwPos, const cIGZString &szOther)
{
    insert(dwPos, szOther.ToChar(), szOther.Strlen());

    return this;
}

cIGZString *cRZString::Replace(uint32_t dwStartPos, const char *pszOther, uint32_t dwLength)
{
    replace(dwStartPos, dwLength, pszOther);

    return this;
}

cIGZString *cRZString::Replace(uint32_t dwStartPos, const cIGZString &szOther)
{
    replace(dwStartPos, szOther.Strlen(), szOther.ToChar());

    return this;
}

cIGZString *cRZString::Erase(uint32_t dwStartPos, uint32_t dwEndPos)
{
    erase(dwStartPos, dwEndPos);

    return this;
}

int32_t cRZString::Find(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const
{
    if (bCaseSensitive) {
        return find(pszOther, dwPos);
    }

    cRZString tmpthis(*this);
    cRZString tmpother(pszOther, strlen(pszOther));
    tmpthis.MakeLower();
    tmpother.MakeLower();

    return tmpthis.find(tmpother, dwPos);
}

int32_t cRZString::Find(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const
{
    if (bCaseSensitive) {
        return find(szOther.ToChar(), dwPos);
    }

    cRZString tmpthis(*this);
    cRZString tmpother(szOther);
    tmpthis.MakeLower();
    tmpother.MakeLower();

    return tmpthis.find(tmpother, dwPos);
}

int32_t cRZString::RFind(const char *pszOther, uint32_t dwPos, bool bCaseSensitive) const
{
    if (bCaseSensitive) {
        return rfind(pszOther, dwPos);
    }

    cRZString tmpthis(*this);
    cRZString tmpother(pszOther, strlen(pszOther));
    tmpthis.MakeLower();
    tmpother.MakeLower();

    return tmpthis.rfind(tmpother, dwPos);
}

int32_t cRZString::RFind(const cIGZString &szOther, uint32_t dwPos, bool bCaseSensitive) const
{
    if (bCaseSensitive) {
        return rfind(szOther.ToChar(), dwPos);
    }

    cRZString tmpthis(*this);
    cRZString tmpother(szOther);
    tmpthis.MakeLower();
    tmpother.MakeLower();

    return tmpthis.rfind(tmpother, dwPos);
}

cIGZString *cRZString::Sprintf(const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    cIGZString *ret = SprintfVaList(pszFormat, args);
    va_end(args);

    return ret;
}

cIGZString *cRZString::SprintfVaList(const char *pszFormat, va_list zList)
{
    char buffer[256];
    int len = std::vsnprintf(buffer, sizeof(buffer), pszFormat, zList);

    if (len < 256) {
        // If we got a positive length in the range of the preallocated buffer we are done.
        // Otherwise start allocating increasingly large buffers until we get a valid return.
        if (len > 0) {
            assign(buffer);
        } else {
            size_t tmp_len = std::max(2 * sizeof(pszFormat), 2 * size());

            while (tmp_len <= 999999 && len < 0) {
                resize(len + 1);
                len = std::vsnprintf(&operator[](0), tmp_len, pszFormat, zList);
                tmp_len *= 2;
            }

            if (len > 0) {
                resize(len);
            }
        }
    } else {
        // If we got a positive length greater than the buffer allocate a temporary buffer of appropriate size.
        resize(len + 1);
        len = std::vsnprintf(&operator[](0), len + 1, pszFormat, zList);
        resize(len);
    }

    return this;
}

void cRZString::MakeLower()
{
    std::transform(begin(), end(), begin(), [](unsigned char c) { return std::tolower((unsigned char)c); });
}

void cRZString::MakeUpper()
{
    std::transform(begin(), end(), begin(), [](unsigned char c) { return std::toupper((unsigned char)c); });
}

void cRZString::LTrim()
{
    LTrim(" \t");
}

void cRZString::RTrim()
{
    RTrim(" \t");
}

void cRZString::Trim()
{
    LTrim();
    RTrim();
}
