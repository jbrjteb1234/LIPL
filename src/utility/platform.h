#ifndef LIPL_UTILITY_PLATFORM_H
#define LIPL_UTILITY_PLATFORM_H

/* The few things that differ between Windows and POSIX. Kept in their own
   file so <windows.h>, whose macros clash with names like CONST, never meets
   the rest of the code. */

#include <stdbool.h>
#include <stddef.h>

/* Size in bytes of the calling thread's stack, or 0 if it can't be found out
   (or is unlimited). */
size_t thread_stack_size(void);

/* Whether stderr is a terminal that understands ANSI colour codes. On Windows
   this also switches the console into the mode that does. */
bool stderr_supports_colour(void);

#endif /* LIPL_UTILITY_PLATFORM_H */
