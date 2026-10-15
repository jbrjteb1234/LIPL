#ifndef LIPL_LEXER_SYMBOL_TABLE_H
#define LIPL_LEXER_SYMBOL_TABLE_H

#include <stddef.h>
#include <stdint.h>

/* Interns identifier names: each distinct name gets a stable integer id, and
   ids can be turned back into names for error messages, printing and builtins.
   It must outlive every token and AST node that refers to an id. */
typedef struct symbol_table{
    char**    names;            /* id -> name, owned */
    uint32_t  count;
    uint32_t  names_capacity;
    int32_t*  buckets;          /* open-addressed hash index: -1 = empty, else an id */
    uint32_t  bucket_count;     /* always a power of two */
} symbol_table;

symbol_table* create_symbol_table(void);

void destroy_symbol_table(symbol_table* table);

/* Returns the id for name[0..length), adding it if it is new. */
int intern_symbol(symbol_table* table, const char* name, size_t length);

/* Returns the name for an id, or "<unknown>" for an id that was never issued. */
const char* symbol_name(const symbol_table* table, int id);

#endif /* LIPL_LEXER_SYMBOL_TABLE_H */
