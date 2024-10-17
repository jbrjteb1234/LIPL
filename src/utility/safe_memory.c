
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "safe_memory.h"

#define MAX_RETRIES 3

void* safe_malloc(size_t size) {
    void* ptr = NULL;
    for (int i = 0; i < MAX_RETRIES; ++i) {
        ptr = malloc(size);
        if (ptr != NULL) {
            return ptr;
        }
        fprintf(stderr, "malloc failed, retrying %d/%d...\n", i + 1, MAX_RETRIES);
    }
    exit(EXIT_FAILURE);
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = NULL;
    if(size <= 0){
        fprintf(stderr, "Invalid size passed to safe_realloc\n");
        return NULL;
    }
    for (int i = 0; i < MAX_RETRIES; ++i) {
        new_ptr = realloc(ptr, size);
        if (new_ptr != NULL) {
            return new_ptr;
        }
    }
    fprintf(stderr, "Failed to reallocate memory after %d retries, exiting.\n", MAX_RETRIES);
    exit(EXIT_FAILURE);
}

void safe_memcpy(void* dest, const void* src, size_t count) {
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "Null pointer passed to safe_memcpy\n");
        exit(EXIT_FAILURE);
    }
    memcpy(dest, src, count);
}

void safe_memset(void* ptr, int value, size_t num) {
    if (ptr == NULL) {
        fprintf(stderr, "Null pointer passed to safe_memset\n");
        exit(EXIT_FAILURE);
    }
    memset(ptr, value, num);
}

void safe_free(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL; // Avoid dangling pointers by setting the pointer to NULL
    }
}