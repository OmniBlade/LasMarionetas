/**
 * @file
 *
 * @brief Array of GIMEX codec implementations.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include <gimex.h>

typedef struct _GimexFunctions
{
    GABOUT *(GIMEX_API *about)();
    int(GIMEX_API *is)(GSTREAM *);
    int(GIMEX_API *open)(GINSTANCE **, GSTREAM *, const char *, bool);
    GINFO *(GIMEX_API *info)(GINSTANCE *, int);
    int(GIMEX_API *read)(GINSTANCE *, GINFO *, char *, int);
    int(GIMEX_API *close)(GINSTANCE *);
    int(GIMEX_API *wopen)(GINSTANCE **, GSTREAM *, const char *, bool);
    int(GIMEX_API *write)(GINSTANCE *, const GINFO *, char *, int);
    int(GIMEX_API *wclose)(GINSTANCE *);
} GimexFunctions;

extern GimexFunctions gFunctions[];
