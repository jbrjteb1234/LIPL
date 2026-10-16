#ifndef LIPL_UTILITY_PLATFORM_H
#define LIPL_UTILITY_PLATFORM_H

#include <stddef.h>

/* Size in bytes of the calling thread's stack, or 0 if it can't be found out
   (or is unlimited). Kept in its own file so <windows.h>, whose macros clash
   with names like CONST, never meets the rest of the code. */
size_t thread_stack_size(void);

#endif /* LIPL_UTILITY_PLATFORM_H */
