// lexer/symbol_table.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"
#include "../utility/safe_memory.h"

#define INITIAL_BUCKET_COUNT 64u
#define INITIAL_NAMES_CAPACITY 32u

/* FNV-1a */
static uint32_t hash_name(const char* name, size_t length){
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < length; ++i){
        hash ^= (unsigned char)name[i];
        hash *= 16777619u;
    }
    return hash;
}

static void mark_buckets_empty(int32_t* buckets, uint32_t count){
    for (uint32_t i = 0; i < count; ++i){
        buckets[i] = -1;
    }
}

symbol_table* create_symbol_table(void){
    symbol_table* table = (symbol_table*)safe_malloc(sizeof(symbol_table));
    table->count = 0;
    table->names_capacity = INITIAL_NAMES_CAPACITY;
    table->names = (char**)safe_malloc(sizeof(char*) * table->names_capacity);
    table->bucket_count = INITIAL_BUCKET_COUNT;
    table->buckets = (int32_t*)safe_malloc(sizeof(int32_t) * table->bucket_count);
    mark_buckets_empty(table->buckets, table->bucket_count);
    return table;
}

void destroy_symbol_table(symbol_table* table){
    if (!table) return;
    for (uint32_t i = 0; i < table->count; ++i){
        safe_free((void**)&table->names[i]);
    }
    safe_free((void**)&table->names);
    safe_free((void**)&table->buckets);
    safe_free((void**)&table);
}

static void grow_buckets(symbol_table* table){
    uint32_t new_count = table->bucket_count * 2u;
    int32_t* buckets = (int32_t*)safe_malloc(sizeof(int32_t) * new_count);
    mark_buckets_empty(buckets, new_count);

    for (uint32_t id = 0; id < table->count; ++id){
        const char* name = table->names[id];
        uint32_t slot = hash_name(name, strlen(name)) & (new_count - 1u);
        while (buckets[slot] != -1){
            slot = (slot + 1u) & (new_count - 1u);
        }
        buckets[slot] = (int32_t)id;
    }

    safe_free((void**)&table->buckets);
    table->buckets = buckets;
    table->bucket_count = new_count;
}

int intern_symbol(symbol_table* table, const char* name, size_t length){
    uint32_t mask = table->bucket_count - 1u;
    uint32_t slot = hash_name(name, length) & mask;

    while (table->buckets[slot] != -1){
        const char* existing = table->names[table->buckets[slot]];
        if (strncmp(existing, name, length) == 0 && existing[length] == '\0'){
            return table->buckets[slot];
        }
        slot = (slot + 1u) & mask;
    }

    /* new name: slot is the empty bucket the probe stopped at */
    if (table->count == table->names_capacity){
        table->names_capacity *= 2u;
        table->names = (char**)safe_realloc(table->names, sizeof(char*) * table->names_capacity);
    }
    int id = (int)table->count;
    table->names[table->count++] = safe_strndup(name, length);
    table->buckets[slot] = id;

    /* keep the load factor at or below one half so probes stay short */
    if (table->count * 2u > table->bucket_count){
        grow_buckets(table);
    }
    return id;
}

const char* symbol_name(const symbol_table* table, int id){
    if (id < 0 || (uint32_t)id >= table->count){
        return "<unknown>";
    }
    return table->names[id];
}
