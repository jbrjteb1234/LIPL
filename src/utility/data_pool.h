// utility/data_pool.h
#ifndef DATA_POOL
#define DATA_POOL

#include <stdint.h>
#include "stack.h"

/*
 * Data pool of fixed-size elements.
 *
 * IMPORTANT:
 * - Free-list stores INDICES (uint32_t), not raw pointers. This makes the pool
 *   robust to base pointer moves during realloc().
 * - acquire_from_pool() returns a pointer computed as base + idx*elem_size.
 * - return_to_pool() validates that the pointer belongs to the pool and is aligned.
 */

typedef struct data_pool {          /* <-- give the struct a tag to match forward decls */
    uint32_t element_size;   // size (bytes) of each element
    uint32_t max;            // current capacity (#elements)
    uint32_t remaining;      // #free elements available
    stack*   free_list;      // stack of uint32_t indices (0..max-1)
    void*    data;           // base pointer of contiguous storage
} data_pool;

data_pool* init_data_pool(uint32_t element_size, uint32_t initial_size);

/** returns pointer to the data pool and pushes to free list
 *  checks if the pointer is aligned and in-range for this pool
 */
void return_to_pool(data_pool* pool, void* data);

void* acquire_from_pool(data_pool* pool);

void reset_pool(data_pool* pool);

void expand_data_pool(data_pool* pool);

void shutdown_data_pool(data_pool* pool);

#endif /* DATA_POOL */
