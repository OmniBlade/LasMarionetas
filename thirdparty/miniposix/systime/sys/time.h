#ifndef MINIPOSIX_SYSTIME_H
#define MINIPOSIX_SYSTIME_H

#ifdef _WIN32

#include <winsock2.h>

struct timezone
{
    int tz_minuteswest; /* of Greenwich */
    int tz_dsttime; /* type of dst correction to apply */
};

int gettimeofday(struct timeval *tp, void *tzp);

#endif

#endif /* MINIPOSIX_SYSTIME_H */
