#ifndef DATA_POOL
#define DATA_POOL

#include "stack.h"
#include <stdint.h>

typedef struct{
    uint32_t element_size;
    uint32_t max;
    uint32_t remaining;
    stack* free_list;
    void* data;
} data_pool;

data_pool* init_data_pool(uint32_t element_size, uint32_t initial_size);

/** returns pointer to the data pool and pushes to free list
 *  checks if the data pointer is actually in the pool
 */
void return_to_pool(data_pool* pool, void* data);

void* acquire_from_pool(data_pool* pool);

void reset_pool(data_pool* pool);

void expand_data_pool(data_pool* pool);

void shutdown_data_pool(data_pool* pool);

#endif