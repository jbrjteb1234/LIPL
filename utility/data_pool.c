#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "safe_memory.h"
#include "data_pool.h"

/** init new data pool
 *  pre-mallocs a block of contiguous data
 */
data_pool* init_data_pool(int element_size, int initial_size){
    data_pool* new_pool = safe_malloc(sizeof(data_pool));
    new_pool->free_list = create_stack(sizeof(void*));
    new_pool->element_size = element_size;
    new_pool->max = initial_size;
    new_pool->remaining = initial_size;
    new_pool->data = safe_malloc(element_size * initial_size);
    for(int i=0; i<initial_size; i++){
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

void reset_pool(data_pool* pool){
    for(int i=0; i<(pool->max - pool->remaining); ++i){
        void* data = pop(pool->free_list);
    }
    pool->remaining = pool->max;
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
    pool->remaining--;
    return pop(pool->free_list);
}

void shutdown_data_pool(data_pool* pool){
    safe_free((void**)&pool->data);
    safe_free((void**)&pool->free_list);
    safe_free((void**)&pool);
}