/**
 * @file
 *
 * @brief Wrappers to set and get locale information.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "rzlocale.h"
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifdef _WIN32
#include <windows.h>
#include <winnls.h>
#endif

static int32_t snCurrentSystemCodePage = kUndefinedCodePage;

char *RZSetlocaleA(int category, char const *locale)
{
    snCurrentSystemCodePage = kUndefinedCodePage;
    return setlocale(category, locale);
}

int32_t GetCurrentSystemCodePage()
{
    if (snCurrentSystemCodePage == kUndefinedCodePage) {
#ifdef _WIN32
        snCurrentSystemCodePage = GetACP();
#elif defined __APPLE__ || defined __linux__ || defined __FreeBSD__
        // These systems all use UTF-8 by default.
        snCurrentSystemCodePage = kUTF8CodePage;
#else // fallback to querying it with setlocale
#pragma message "No code page detection method was defined for this platform so setlocale will be used."
        snCurrentSystemCodePage = kDefaultCodePage;

        // Interrogate the current locale.
        char *current_locale = setlocale(LC_ALL, nullptr);

        if (current_locale) {
            char *current_cp = current_locale;
            if (strcmp(current_cp, "C") != 0 || (current_cp = setlocale(LC_ALL, "")) != nullptr) {
                current_cp = strchr(current_cp, '.');

                if (current_cp != nullptr) {
                    if (strncasecmp(current_cp + 1, "utf", 3) != 0) {
                        snCurrentSystemCodePage = kUTF8CodePage;
                    } else if (isdigit(uint8_t(current_cp[1]))) {
                        snCurrentSystemCodePage = atoi(current_cp + 1);
                    }
                }

                // restore the current locale.
                setlocale(LC_ALL, current_locale);
            }
        }
#endif
    }

    return snCurrentSystemCodePage;
}

int32_t GetCurrentFrameworkCodePage()
{
    return kUTF8CodePage;
}
