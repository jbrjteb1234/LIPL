// utility/data_pool.c
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "safe_memory.h"
#include "data_pool.h"

struct data_pool_chunk {
    data_pool_chunk* next;
    uint32_t         capacity;
    unsigned char*   elements;
};

/* Every element must be aligned for any type that might be stored in it */
static size_t round_up_to_alignment(size_t size){
    const size_t alignment = _Alignof(max_align_t);
    return (size + alignment - 1u) / alignment * alignment;
}

/* push in reverse so elements are handed out in address order */
static void push_chunk_elements(data_pool* pool, data_pool_chunk* chunk){
    for (uint32_t i = chunk->capacity; i > 0; --i){
        push(pool->free_list, chunk->elements + (size_t)(i - 1u) * pool->element_size, false);
    }
}

static void add_chunk(data_pool* pool, uint32_t capacity){
    if (capacity > UINT32_MAX - pool->max || (size_t)capacity > SIZE_MAX / pool->element_size) {
        fprintf(stderr, "fatal: data pool capacity overflow\n");
        exit(EXIT_FAILURE);
    }

    data_pool_chunk* chunk = (data_pool_chunk*)safe_malloc(sizeof(data_pool_chunk));
    chunk->capacity = capacity;
    chunk->elements = (unsigned char*)safe_malloc((size_t)capacity * pool->element_size);
    chunk->next = pool->chunks;
    pool->chunks = chunk;

    push_chunk_elements(pool, chunk);
    pool->max += capacity;
    pool->remaining += capacity;
}

/** init new data pool: pre-mallocs a first chunk of initial_size elements */
data_pool* init_data_pool(size_t element_size, uint32_t initial_size){
    if (element_size == 0 || initial_size == 0) {
        fprintf(stderr, "init_data_pool: invalid args (element_size=%u, initial_size=%u)\n",
                (unsigned)element_size, (unsigned)initial_size);
        exit(EXIT_FAILURE);
    }

    data_pool* new_pool = (data_pool*)safe_malloc(sizeof(data_pool));
    new_pool->element_size = round_up_to_alignment(element_size);
    new_pool->max          = 0;
    new_pool->remaining    = 0;
    new_pool->chunks       = NULL;
    new_pool->free_list    = create_stack(sizeof(void*));

    add_chunk(new_pool, initial_size);
    return new_pool;
}

void* acquire_from_pool(data_pool* pool){
    if (pool->remaining == 0){
        // double the total capacity with a new chunk; existing chunks stay where they are
        add_chunk(pool, pool->max);
    }

    void* element;
    memcpy(&element, pop(pool->free_list), sizeof(void*));
    pool->remaining--;

    memset(element, 0, pool->element_size);
    return element;
}

static bool pool_owns(const data_pool* pool, const void* data){
    uintptr_t address = (uintptr_t)data;
    for (const data_pool_chunk* chunk = pool->chunks; chunk != NULL; chunk = chunk->next){
        uintptr_t start = (uintptr_t)chunk->elements;
        uintptr_t end   = start + (uintptr_t)chunk->capacity * pool->element_size;
        if (address >= start && address < end){
            return (address - start) % pool->element_size == 0;
        }
    }
    return false;
}

void return_to_pool(data_pool* pool, void* data){
    if (pool == NULL || data == NULL) {
        return;
    }
    if (!pool_owns(pool, data)) {
        fprintf(stderr, "return_to_pool: %p is not an element of this pool\n", data);
        return;
    }
    push(pool->free_list, data, false);
    pool->remaining++;
}

void reset_pool(data_pool* pool){
    if (!pool) return;
    pool->free_list->top = -1;
    for (data_pool_chunk* chunk = pool->chunks; chunk != NULL; chunk = chunk->next){
        push_chunk_elements(pool, chunk);
    }
    pool->remaining = pool->max;
}

void shutdown_data_pool(data_pool* pool){
    if (!pool) return;
    data_pool_chunk* chunk = pool->chunks;
    while (chunk != NULL){
        data_pool_chunk* next = chunk->next;
        safe_free((void**)&chunk->elements);
        safe_free((void**)&chunk);
        chunk = next;
    }
    destroy_stack(pool->free_list);
    safe_free((void**)&pool);
}
