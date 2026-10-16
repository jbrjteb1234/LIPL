// utility/platform.c - the few things that differ between Windows and POSIX
#ifdef _WIN32

/* GetCurrentThreadStackLimits needs Windows 8 or later */
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0602
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform.h"

size_t thread_stack_size(void){
    ULONG_PTR low = 0;
    ULONG_PTR high = 0;
    GetCurrentThreadStackLimits(&low, &high);
    return high > low ? (size_t)(high - low) : 0;
}

#else

/* getrlimit is POSIX, which -std=c11 hides unless asked for */
#define _POSIX_C_SOURCE 200809L
#include <sys/resource.h>

#include "platform.h"

size_t thread_stack_size(void){
    struct rlimit limit;
    if (getrlimit(RLIMIT_STACK, &limit) != 0 || limit.rlim_cur == RLIM_INFINITY){
        return 0;
    }
    return (size_t)limit.rlim_cur;
}

#endif
