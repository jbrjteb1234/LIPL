#ifndef LIPL_UTILITY_SAFE_MEMORY_H
#define LIPL_UTILITY_SAFE_MEMORY_H

#include <stddef.h>

/* Allocation helpers that never return NULL: running out of memory prints a
   message and exits, so callers don't have to check results. */
void* safe_malloc(size_t size);

void* safe_realloc(void* ptr, size_t size);

/* Copies the first `length` bytes of `source` into a new NUL-terminated string. */
char* safe_strndup(const char* source, size_t length);

void safe_memcpy(void* dest, const void* src, size_t count);

void safe_memset(void* ptr, int value, size_t count);

/* Frees *ptr and sets it to NULL. */
void safe_free(void** ptr);

#endif /* LIPL_UTILITY_SAFE_MEMORY_H */
