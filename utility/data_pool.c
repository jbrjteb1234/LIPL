#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "safe_memory.h"
#include "data_pool.h"

#define INITIAL_POOL_SIZE 20

/** init new data pool
 *  pre-mallocs a block of contiguous data
 */
data_pool* init_data_pool(int element_size){
    data_pool* new_pool = safe_malloc(sizeof(data_pool));
    new_pool->free_list = create_stack(sizeof(void*));
    new_pool->element_size = element_size;
    new_pool->max = INITIAL_POOL_SIZE;
    new_pool->remaining = INITIAL_POOL_SIZE;
    new_pool->data = safe_malloc(element_size * INITIAL_POOL_SIZE);
    for(int i=0; i<INITIAL_POOL_SIZE; i++){
        push(new_pool->free_list, new_pool->data + i * element_size);
    }
    return new_pool;
}

/** returns pointer to the data pool and pushes to free list
 *  checks if the data pointer is actually in the pool
 */
void return_to_pool(data_pool* pool, void* data){
    if (data>pool->data && data < pool->data + pool->max*pool->element_size){
        push(pool->free_list, data);
        pool->remaining++;
    }
}

void expand_data_pool(data_pool* pool){
    pool->max *= 2;
    pool->data = safe_realloc(pool->data, pool->max * pool->element_size);
    pool->remaining = pool->max/2;
    for(int i=0; i<pool->max; i++){
        push(pool->free_list, pool->data + i * pool->element_size);
    }
}

void* acquire_from_pool(data_pool* pool){
    if(pool->remaining == 0){
        expand_data_pool(pool);
    }
    return pop(pool->free_list);
}

void shutdown_data_pool(data_pool* pool){
    safe_free((void**)&pool->data);
    safe_free((void**)&pool->free_list);
    safe_free((void**)&pool);
}