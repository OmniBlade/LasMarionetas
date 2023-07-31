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

cRZString::cRZString(const string_data &source) : string_data(source), mnRefCount(0) {}

cRZString::cRZString(const cRZString &source, size_t dwStart, size_t dwEnd) :
    string_data(source, dwStart, dwEnd), mnRefCount(0)
{
}

cRZString::cRZString(const char *source) : string_data(source), mnRefCount(0) {}

cRZString::cRZString(const char *source, size_t dwLen) : string_data(source, dwLen), mnRefCount(0) {}

cRZString::cRZString(const cIGZString &source) : string_data(source.Data(), source.Strlen()), mnRefCount(0) {}

cRZString::cRZString(char character, size_t repetitions) : string_data(repetitions, character), mnRefCount(0) {}

bool cRZString::QueryInterface(uint32_t riid, void **obj)
{
    switch (riid) {
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            AddRef();
            return true;
        case GZIID_cIGZString:
            *obj = static_cast<cIGZString *>(this);
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

void cRZString::FromChar(const char *source)
{
    if (source == nullptr) {
        erase();
    } else {
        assign(source);
    }
}

void cRZString::FromChar(const char *source, uint32_t length)
{
    if (source == nullptr) {
        erase();
    } else {
        assign(source, length);
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

bool cRZString::IsEqual(const cIGZString *other, bool case_sensitive) const
{
    if (other == nullptr) {
        return Strlen() == 0;
    }

    cRZString tmp(other->ToChar());

    return CompareTo(tmp, case_sensitive) == 0;
}

bool cRZString::IsEqual(const cIGZString &other, bool case_sensitive) const
{
    return CompareTo(other, case_sensitive) == 0;
}

bool cRZString::IsEqual(const char *other, bool case_sensitive) const
{
    if (other == nullptr) {
        return Strlen() == 0;
    }

    return CompareTo(other, strlen(other), case_sensitive) == 0;
}

bool cRZString::IsEqual(const char *other, uint32_t length, bool case_sensitive) const
{
    if (other == nullptr) {
        return Strlen() == 0;
    }

    return CompareTo(other, length, case_sensitive) == 0;
}

int32_t cRZString::CompareTo(const cIGZString &other, bool case_sensitive) const
{
    uint32_t other_len = other.Strlen();
    uint32_t cmp_len = std::min((uint32_t)size(), other_len);

    int32_t ret;

    if (case_sensitive) {
        ret = std::strncmp(c_str(), other.ToChar(), cmp_len);
    } else {
        ret = strncasecmp(c_str(), other.ToChar(), cmp_len);
    }

    if (ret == 0) {
        ret = size() - other_len;
    }

    return ret;
}

int32_t cRZString::CompareTo(const char *other, bool case_sensitive) const
{
    return CompareTo(other, std::strlen(other), case_sensitive);
}

int32_t cRZString::CompareTo(const char *other, uint32_t length, bool case_sensitive) const
{
    if (length == -1) {
        length = std::strlen(other);
    }

    uint32_t cmp_len = std::min(uint32_t(size()), length);
    int32_t ret;

    if (case_sensitive) {
        ret = std::strncmp(c_str(), other, cmp_len);
    } else {
        ret = strncasecmp(c_str(), other, cmp_len);
    }

    if (ret == 0) {
        ret = size() - length;
    }

    return ret;
}

cIGZString &cRZString::operator=(const cIGZString &other)
{
    Copy(other);

    return *this;
}

void cRZString::Copy(const cIGZString &other)
{
    FromChar(other.Data(), other.Strlen());
}

void cRZString::Resize(uint32_t new_size)
{
    resize(new_size);
}

cIGZString *cRZString::Append(const char *other)
{
    if (other != nullptr) {
        append(other, strlen(other));
    }

    return this;
}

cIGZString *cRZString::Append(const char *other, uint32_t length)
{
    append(other, length);

    return this;
}

cIGZString *cRZString::Append(const cIGZString &other)
{
    append(other.ToChar(), other.Strlen());

    return this;
}

cIGZString *cRZString::Insert(uint32_t position, const char *other, uint32_t length)
{
    insert(position, other, length);

    return this;
}

cIGZString *cRZString::Insert(uint32_t position, const cIGZString &other)
{
    insert(position, other.ToChar(), other.Strlen());

    return this;
}

cIGZString *cRZString::Replace(uint32_t start_position, const char *other, uint32_t length)
{
    replace(start_position, length, other);

    return this;
}

cIGZString *cRZString::Replace(uint32_t start_position, const cIGZString &other)
{
    replace(start_position, other.Strlen(), other.ToChar());

    return this;
}

cIGZString *cRZString::Erase(uint32_t start_position, uint32_t end_position)
{
    erase(start_position, end_position);

    return this;
}

int32_t cRZString::Find(const char *other, uint32_t position, bool case_sensitive) const
{
    if (case_sensitive) {
        return find(other, position);
    }

    cRZString tmpthis(*this);
    cRZString tmother(other, strlen(other));
    tmpthis.MakeLower();
    tmother.MakeLower();

    return tmpthis.find(tmother, position);
}

int32_t cRZString::Find(const cIGZString &other, uint32_t position, bool case_sensitive) const
{
    if (case_sensitive) {
        return find(other.ToChar(), position);
    }

    cRZString tmpthis(*this);
    cRZString tmother(other);
    tmpthis.MakeLower();
    tmother.MakeLower();

    return tmpthis.find(tmother, position);
}

int32_t cRZString::RFind(const char *other, uint32_t position, bool case_sensitive) const
{
    if (case_sensitive) {
        return rfind(other, position);
    }

    cRZString tmpthis(*this);
    cRZString tmother(other, strlen(other));
    tmpthis.MakeLower();
    tmother.MakeLower();

    return tmpthis.rfind(tmother, position);
}

int32_t cRZString::RFind(const cIGZString &other, uint32_t position, bool case_sensitive) const
{
    if (case_sensitive) {
        return rfind(other.ToChar(), position);
    }

    cRZString tmpthis(*this);
    cRZString tmother(other);
    tmpthis.MakeLower();
    tmother.MakeLower();

    return tmpthis.rfind(tmother, position);
}

cIGZString *cRZString::Sprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cIGZString *ret = SprintfVaList(format, args);
    va_end(args);

    return ret;
}

cIGZString *cRZString::SprintfVaList(const char *format, va_list zList)
{
    char buffer[256];
    int len = std::vsnprintf(buffer, sizeof(buffer), format, zList);

    if (len < 256) {
        // If we got a positive length in the range of the preallocated buffer we are done.
        // Otherwise start allocating increasingly large buffers until we get a valid return.
        if (len > 0) {
            assign(buffer);
        } else {
            size_t tmp_len = std::max(2 * sizeof(format), 2 * size());

            while (tmp_len <= 999999 && len < 0) {
                resize(len + 1);
                len = std::vsnprintf(&operator[](0), tmp_len, format, zList);
                tmp_len *= 2;
            }

            if (len > 0) {
                resize(len);
            }
        }
    } else {
        // If we got a positive length greater than the buffer allocate a temporary buffer of appropriate size.
        resize(len + 1);
        len = std::vsnprintf(&operator[](0), len + 1, format, zList);
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
