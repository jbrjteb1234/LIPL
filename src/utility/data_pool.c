// utility/data_pool.c
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "safe_memory.h"
#include "data_pool.h"

/* Small helper to guard size_t multiplication overflow */
static int mul_overflow_size_t(size_t a, size_t b, size_t* out) {
    if (a == 0 || b == 0) { *out = 0; return 0; }
    if (a > SIZE_MAX / b)  { return 1; }
    *out = a * b;
    return 0;
}

/** init new data pool: pre-mallocs a block of contiguous data */
data_pool* init_data_pool(uint32_t element_size, uint32_t initial_size){
    if (element_size == 0 || initial_size == 0) {
        fprintf(stderr, "init_data_pool: invalid args (element_size=%u, initial_size=%u)\n",
                element_size, initial_size);
        exit(EXIT_FAILURE);
    }

    data_pool* new_pool = (data_pool*)safe_malloc(sizeof(data_pool));
    new_pool->element_size = element_size;
    new_pool->max          = initial_size;
    new_pool->remaining    = initial_size;

    size_t bytes = 0;
    if (mul_overflow_size_t((size_t)element_size, (size_t)initial_size, &bytes)) {
        fprintf(stderr, "init_data_pool: size overflow\n");
        exit(EXIT_FAILURE);
    }
    new_pool->data = safe_malloc(bytes);

    // Free list stores indices (uint32_t)
    new_pool->free_list = create_stack(sizeof(uint32_t));

    // Push indices 0..initial_size-1
    for(uint32_t i = 0; i < initial_size; ++i){
        push(new_pool->free_list, &i, true); // copy by value
    }
    return new_pool;
}

/** returns pointer to the data pool and pushes to free list */
void return_to_pool(data_pool* pool, void* data){
    if (pool == NULL || data == NULL || pool->data == NULL) {
        return;
    }
    // Compute offset from base and validate alignment
    uintptr_t base = (uintptr_t)pool->data;
    uintptr_t ptr  = (uintptr_t)data;

    if (ptr < base) return;

    size_t off = (size_t)(ptr - base);
    if (pool->element_size == 0) return; // should never happen
    if (off % pool->element_size != 0) return;

    uint32_t idx = (uint32_t)(off / pool->element_size);
    if (idx >= pool->max) return;

    push(pool->free_list, &idx, true);
    pool->remaining++;
}

void reset_pool(data_pool* pool){
    if (!pool) return;
    // Reset free list to contain all indices 0..max-1
    pool->free_list->top = -1;
    for (uint32_t i = 0; i < pool->max; ++i) {
        push(pool->free_list, &i, true);
    }
    pool->remaining = pool->max;
}

void expand_data_pool(data_pool* pool)
{
    if (!pool) return;

    // Prevent 32-bit overflow on max doubling
    if (pool->max > (UINT32_MAX / 2u)) {
        fprintf(stderr, "expand_data_pool: capacity overflow\n");
        exit(EXIT_FAILURE);
    }

    uint32_t old_max = pool->max;
    uint32_t new_max = old_max * 2u;

    // Realloc contiguous memory
    size_t bytes = 0;
    if (mul_overflow_size_t((size_t)new_max, (size_t)pool->element_size, &bytes)) {
        fprintf(stderr, "expand_data_pool: size overflow\n");
        exit(EXIT_FAILURE);
    }
    pool->data = safe_realloc(pool->data, bytes);

    // Push new indices [old_max, new_max)
    for (uint32_t i = old_max; i < new_max; i++) {
        push(pool->free_list, &i, true);
    }

    pool->remaining += (new_max - old_max);
    pool->max        = new_max;
}

void* acquire_from_pool(data_pool* pool){
    if (!pool) return NULL;
    if (pool->remaining == 0){
        expand_data_pool(pool);
    }
    pool->remaining--;

    void* top = pop(pool->free_list);
    if (!top) {
        fprintf(stderr, "acquire_from_pool: free_list underflow\n");
        exit(EXIT_FAILURE);
    }
    uint32_t idx = *(uint32_t*)top;

    return (uint8_t*)pool->data + (size_t)idx * pool->element_size;
}

void shutdown_data_pool(data_pool* pool){
    if (!pool) return;
    safe_free((void**)&pool->data);
    safe_free((void**)&pool->free_list->data);
    safe_free((void**)&pool->free_list);
    safe_free((void**)&pool);
}
