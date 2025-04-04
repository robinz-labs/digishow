#ifndef WIN32_COMPAT_H
#define WIN32_COMPAT_H

#ifdef _WIN32
#include <windows.h>

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct _timeval {
    long tv_sec;
    long tv_usec;
};
#define timeval _timeval
#endif

static int gettimeofday(struct timeval* tp, void* tzp) {
    FILETIME ft;
    LARGE_INTEGER li;
    __int64 t;

    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    t = li.QuadPart;
    t -= 116444736000000000LL;
    t /= 10;
    tp->tv_sec = (long)(t / 1000000);
    tp->tv_usec = (long)(t % 1000000);
    return 0;
}

#endif // _WIN32
#endif // WIN32_COMPAT_H