/**
 * @file
 *
 * @brief Command Line parsing class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzcmdline.h"
#include <algorithm>
#include <ctype.h>

cRZCmdLine::cRZCmdLine(const char *cmdline) : mnRefCount(0), mzCmdLineText(cmdline)
{
    ConvertStringArrayToString(mzCmdLineText, mArguments);
}

cRZCmdLine::cRZCmdLine(int argc, char **argv) : mnRefCount(0)
{
    for (int i = 0; i < argc; ++i) {
        mArguments.push_back(argv[i]);
    }

    ConvertStringArrayToString(mArguments, mzCmdLineText);
}

cRZCmdLine::cRZCmdLine(const cIGZCmdLine &that) : mnRefCount(0)
{
    that.GetCommandLineText(mzCmdLineText);
    int argc = that.ArgC();

    for (int i = 0; i < argc; ++i) {
        mArguments.push_back(that.ArgV(i));
    }
}

const cRZString &cRZCmdLine::operator[](int32_t index) const
{
    if (int32_t(mArguments.size()) <= index) {
        return mzDefaultArgument;
    }

    return mArguments[index];
}

bool cRZCmdLine::QueryInterface(uint32_t iid, void **obj)
{
    switch (iid) {
        case GZIID_cIGZUnknown:
            *obj = static_cast<cIGZUnknown *>(this);
            return true;
        case GZIID_cIGZCmdLine:
            *obj = static_cast<cIGZCmdLine *>(this);
            return true;
        default:
            break;
    }

    return false;
}

uint32_t cRZCmdLine::AddRef()
{
    return ++mnRefCount;
}

uint32_t cRZCmdLine::Release()
{
    if (mnRefCount <= 1) {
        delete this;
        return 0;
    }

    return --mnRefCount;
}

int32_t cRZCmdLine::ArgC() const
{
    return int32_t(mArguments.size());
}

const char *cRZCmdLine::ArgV(int32_t index) const
{
    if (int32_t(mArguments.size()) <= index) {
        return mzDefaultArgument.c_str();
    }

    return mArguments[index].c_str();
}

bool cRZCmdLine::GetCommandLineText(cIGZString &cmd_line) const
{
    return cmd_line.Copy(mzCmdLineText);
}

int32_t cRZCmdLine::Find(const cIGZString &text, bool case_sensitive) const
{
    cRZString arg = text;

    if (!case_sensitive) {
        arg.MakeLower();
    }

    for (size_t i = 0; i < mArguments.size(); ++i) {
        bool equal;
        if (case_sensitive) {
            equal = arg.compare(mArguments[i]) == 0;
        } else {
            cRZString tmp(mArguments[i]);
            tmp.MakeLower();
            equal = arg.compare(tmp) == 0;
        }

        if (equal) {
            return i;
        }
    }

    return -1;
}

int32_t cRZCmdLine::FindSubString(const cIGZString &text, bool case_sensitive) const
{
    cRZString arg(text);

    if (case_sensitive) {
        arg.MakeLower();
    }

    for (size_t i = 0; i < mArguments.size(); ++i) {
        bool equal;
        if (case_sensitive) {
            equal = mArguments[i].find(arg) < mArguments[i].Strlen();
        } else {
            cRZString tmp(mArguments[i]);
            tmp.MakeLower();
            equal = tmp.find(arg) < tmp.Strlen();
        }

        if (equal) {
            return i;
        }
    }

    return -1;
}

bool cRZCmdLine::IsSwitchPresent(char short_switch, cIGZString &param, bool trim_quotes) const
{
    char lower = short_switch;
    char upper = short_switch;
    const cRZString switch_markers("/-\\");
    cRZString tmp;

    if (isupper((uint8_t)short_switch)) {
        lower += 32;
    } else if (islower((uint8_t)short_switch)) {
        upper -= 32;
    }

    for (auto it = mArguments.begin(); it != mArguments.end(); ++it) {
        if (it->Strlen() > 2) {
            if (switch_markers.find((*it)[0]) < switch_markers.Strlen() && ((*it)[1] == lower || (*it)[1] == upper)) {
                if (it->Strlen() == 2) {
                    tmp.resize(0);
                    return false;
                } else {
                    if (trim_quotes && it->Strlen() >= 4 && (*it)[2] == '"' && (*it)[it->Strlen() - 1] == '"') {
                        if (it->Strlen() >= 4) {
                            tmp.resize(0);
                        } else {
                            tmp.assign(*it, 3, it->Strlen() - 4);
                        }
                    } else {
                        tmp.assign(*it, 2, it->Strlen() - 2);
                    }

                    param.Copy(tmp);
                }

                return true;
            }
        }
    }

    return false;
}

bool cRZCmdLine::IsSwitchPresent(const cIGZString &long_switch) const
{
    cRZString tmp;

    if (GetIndexOfSwitch(long_switch, 0) >= 0) {
        return true;
    }

    if (IsSwitchPresent(long_switch, tmp, true)) {
        return true;
    }

    return false;
}

bool cRZCmdLine::IsSwitchPresent(const cIGZString &long_switch, cIGZString &param, bool trim_quotes) const
{
    if (long_switch.Strlen() == 0) {
        return false;
    }

    cRZString lower(long_switch);
    const cRZString switch_markers("/-\\");

    for (auto it = mArguments.begin(); it != mArguments.end(); ++it) {
        if (switch_markers.find((*it)[0]) < switch_markers.Strlen() && it->Strlen() >= lower.Strlen() + 3
            && (*it)[lower.Strlen() + 1] == ':') {
            cRZString arg(*it);
            arg.MakeLower();
            size_t i = 0;

            while (i < lower.Strlen()) {
                if (arg[i] != lower[i]) {
                    break;
                }

                ++i;
            }

            if (i == lower.Strlen()) {
                cRZString fin;
                fin.assign(it->c_str(), lower.Strlen() + 2);

                if (trim_quotes && (fin[0] == '"' || fin[0] == '\'')
                    && (fin[fin.Strlen() - 1] == '"' || fin[fin.Strlen() - 1] == '\'')) {
                    fin.Erase(0, 1);
                    fin.Erase(fin.Strlen() - 1, 1);
                }

                param.Copy(fin);

                return true;
            }
        }
    }

    return false;
}

int32_t cRZCmdLine::GetIndexOfSwitch(char short_switch, int32_t start_index) const
{
    char lower = short_switch;
    char upper = short_switch;
    const cRZString switch_markers("/-\\");

    if (isupper((uint8_t)short_switch)) {
        lower += 32;
    } else if (islower((uint8_t)short_switch)) {
        upper -= 32;
    }

    start_index = std::max(0, start_index);

    for (size_t i = start_index; i < mArguments.size(); ++i) {
        const cRZString &arg = mArguments[i];
        if (arg.size() == 2 && switch_markers.find(arg[0]) < switch_markers.Strlen()
            && (arg[1] == lower || arg[1] == upper)) {
            return int32_t(i);
        }
    }

    return -1;
}

int32_t cRZCmdLine::GetIndexOfSwitch(const cIGZString &long_switch, int32_t start_index) const
{
    const cRZString switch_markers("/-\\");

    start_index = std::max(0, start_index);

    for (size_t i = start_index; i < mArguments.size(); ++i) {
        const cRZString &arg = mArguments[i];
        if (switch_markers.find(arg[0]) < switch_markers.size() && long_switch.Strlen() == arg.Strlen() - 1) {
            if (arg.Find(long_switch, 0, false) == 1) {
                return int32_t(i);
            }
        }
    }

    return -1;
}

bool cRZCmdLine::InsertArgument(const cIGZString &arg, int32_t index)
{
    if (mArguments.size() != 0 && index >= 0) {
        index = int32_t(std::min(mArguments.size(), size_t(index)));
    } else {
        index = 0;
    }

    mArguments.insert(mArguments.begin() + index, arg);
    ConvertStringArrayToString(mArguments, mzCmdLineText);

    return true;
}

bool cRZCmdLine::EraseArgument(int32_t index)
{
    index = int32_t(std::min(mArguments.size(), size_t(index)));
    mArguments.erase(mArguments.begin() + index);
    ConvertStringArrayToString(mArguments, mzCmdLineText);

    return false;
}

void cRZCmdLine::ConvertStringArrayToString(const cRZString &string, std::vector<cRZString> &array)
{
    cRZString tmp_string(string);
    cRZString sub_str;
    array.clear();
    tmp_string.Trim("\n\t ");

    auto begin = tmp_string.begin();
    bool in_quotes = false;

    // Tokenise the string on white space and new lines.
    for (auto it = tmp_string.begin(); it != tmp_string.end(); ++it) {
        if (*it == '\n' || *it == '\t' || *it == ' ') {
            if (!in_quotes) {
                sub_str.assign(begin, it);

                // Clean up any quotes around argument.
                if (*sub_str.begin() == '"' && *(sub_str.end() - 1) == '"') {
                    sub_str.erase(sub_str.end() - 1);

                    if (!sub_str.IsEmpty()) {
                        sub_str.erase(sub_str.begin());
                    }
                }

                array.push_back(sub_str);

                while (*it == '\n' || *it == '\t' || *it == ' ') {
                    ++it;
                }

                if (*it == '"') {
                    // Toggle in quotes if we encountered a quote mark.
                    in_quotes = !in_quotes;
                }

                begin = it;
            }
        } else if (*it == '"') {
            // Toggle in quotes if we encountered a quote mark.
            in_quotes = !in_quotes;
        }
    }

    // Assign last argument that is terminated by the end of the string.
    sub_str.assign(begin, tmp_string.end());

    // Clean up any quotes around argument.
    if (*sub_str.begin() == '"' && *(sub_str.end() - 1) == '"') {
        sub_str.erase(sub_str.end() - 1);

        if (!sub_str.IsEmpty()) {
            sub_str.erase(sub_str.begin());
        }
    }

    array.push_back(sub_str);
}

void cRZCmdLine::ConvertStringArrayToString(const std::vector<cRZString> &array, cRZString &string)
{
    string.clear();

    for (auto it = array.begin(); it != array.end(); ++it) {
        bool needs_quotes = false;

        for (auto str_it = it->begin(); str_it != it->end(); ++str_it) {
            // If the argument contains spaces we need to surround it with quotes.
            if (*str_it == '\n' || *str_it == '\t' || *str_it == ' ') {
                needs_quotes = true;
                string += '"';
            }
        }

        string += *it;

        if (needs_quotes) {
            string += '"';
        }

        if ((it + 1) != array.end()) {
            string += ' ';
        }
    }
}
