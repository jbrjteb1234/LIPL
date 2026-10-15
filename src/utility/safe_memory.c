// utility/safe_memory.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "safe_memory.h"

static void out_of_memory(void) {
    fprintf(stderr, "fatal: out of memory\n");
    exit(EXIT_FAILURE);
}

void* safe_malloc(size_t size) {
    /* malloc(0) is allowed to return NULL, which would look like a failure */
    void* ptr = malloc(size ? size : 1u);
    if (ptr == NULL) {
        out_of_memory();
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size ? size : 1u);
    if (new_ptr == NULL) {
        out_of_memory();
    }
    return new_ptr;
}

char* safe_strndup(const char* source, size_t length) {
    char* copy = safe_malloc(length + 1u);
    safe_memcpy(copy, source, length);
    copy[length] = '\0';
    return copy;
}

void safe_memcpy(void* dest, const void* src, size_t count) {
    if (count == 0) {
        return;
    }
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "fatal: null pointer passed to safe_memcpy\n");
        exit(EXIT_FAILURE);
    }
    memcpy(dest, src, count);
}

void safe_memset(void* ptr, int value, size_t count) {
    if (count == 0) {
        return;
    }
    if (ptr == NULL) {
        fprintf(stderr, "fatal: null pointer passed to safe_memset\n");
        exit(EXIT_FAILURE);
    }
    memset(ptr, value, count);
}

void safe_free(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL; // Avoid dangling pointers by setting the pointer to NULL
    }
}
