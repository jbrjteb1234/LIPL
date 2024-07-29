#ifndef SAFE_MALLOC
#define SAFE_MALLOC

void* safe_malloc(size_t);

void* safe_realloc(size_t);

void safe_memcpy(void* dest, const void* src, size_t count);

#endif