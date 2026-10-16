// utility/platform.c - the few things that differ between Windows and POSIX
#ifdef _WIN32

/* GetCurrentThreadStackLimits needs Windows 8 or later */
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0602
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <stdio.h>

#include "platform.h"

/* Windows 10's name for the console mode that understands colour codes */
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

size_t thread_stack_size(void){
    ULONG_PTR low = 0;
    ULONG_PTR high = 0;
    GetCurrentThreadStackLimits(&low, &high);
    return high > low ? (size_t)(high - low) : 0;
}

bool stderr_supports_colour(void){
    if (!_isatty(_fileno(stderr))){
        return false;
    }
    /* _isatty is also true for NUL; only a real console has a console mode */
    HANDLE console = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;
    if (console == INVALID_HANDLE_VALUE || !GetConsoleMode(console, &mode)){
        return false;
    }
    return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0
        || SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

#else

/* getrlimit and isatty are POSIX, which -std=c11 hides unless asked for */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#include "platform.h"

size_t thread_stack_size(void){
    struct rlimit limit;
    if (getrlimit(RLIMIT_STACK, &limit) != 0 || limit.rlim_cur == RLIM_INFINITY){
        return 0;
    }
    return (size_t)limit.rlim_cur;
}

bool stderr_supports_colour(void){
    const char* term = getenv("TERM");
    return isatty(STDERR_FILENO) && !(term != NULL && strcmp(term, "dumb") == 0);
}

#endif
