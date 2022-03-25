/**
 * @file
 *
 * @brief Embedded git information.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#ifndef COMMON_GITINFO_H
#define COMMON_GITINFO_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

extern const char GitSHA1[];
extern const char GitShortSHA1[];
extern const char GitCommitDate[];
extern const char GitCommitAuthorName[];
extern const char GitTag[];
extern time_t GitCommitTimeStamp;
extern bool GitUncommittedChanges;
extern bool GitHaveInfo;
extern int GitRevision;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
