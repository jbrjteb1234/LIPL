// utility/data_pool.h
#ifndef LIPL_UTILITY_DATA_POOL_H
#define LIPL_UTILITY_DATA_POOL_H

#include <stddef.h>
#include <stdint.h>
#include "stack.h"

/*
 * Pool of fixed-size elements.
 *
 * Storage grows by adding new chunks. Existing chunks are never moved or
 * reallocated, so a pointer returned by acquire_from_pool() stays valid until
 * the pool is reset or shut down. (Growing one block with realloc() would move
 * every element and leave callers holding dangling pointers.)
 */
typedef struct data_pool_chunk data_pool_chunk;

typedef struct data_pool {
    size_t           element_size;  // bytes per element, rounded up for alignment
    uint32_t         max;           // total elements across all chunks
    uint32_t         remaining;     // elements currently on the free list
    stack*           free_list;     // stack of pointers to free elements
    data_pool_chunk* chunks;        // newest chunk first
} data_pool;

data_pool* init_data_pool(size_t element_size, uint32_t initial_size);

/** returns a zero-filled element; never returns NULL */
void* acquire_from_pool(data_pool* pool);

/** hands an element back for reuse. Pointers that don't belong to this pool
 *  are reported and ignored.
 */
void return_to_pool(data_pool* pool, void* data);

/** marks every element free again; pointers handed out earlier must not be used afterwards */
void reset_pool(data_pool* pool);

void shutdown_data_pool(data_pool* pool);

#endif /* LIPL_UTILITY_DATA_POOL_H */
