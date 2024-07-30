#ifndef SAFE_MALLOC
#define SAFE_MALLOC

void* safe_malloc(size_t);

void* safe_realloc(void*, size_t);

void safe_memcpy(void* dest, const void* src, size_t count);

void safe_memset(void*, int, size_t );

void safe_free(void* ptr);

#endif