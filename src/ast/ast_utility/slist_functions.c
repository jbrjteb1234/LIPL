#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "slist_functions.h"
#include "../../utility/safe_memory.h"

#define INITIAL_SLIST_SIZE 8u

statement_list* create_new_slist(void) {
    statement_list* new_slist = (statement_list*)safe_malloc(sizeof(statement_list));
    new_slist->list  = (ASTNode**)safe_malloc(sizeof(ASTNode*) * INITIAL_SLIST_SIZE);
    new_slist->max   = INITIAL_SLIST_SIZE;
    new_slist->count = 0;
    return new_slist;
}

void destroy_slist(statement_list* slist) {
    if (!slist) return;
    safe_free((void**)&slist->list);
    safe_free((void**)&slist);
}

static void expand_slist(statement_list* slist) {
    if (slist->max > UINT32_MAX / 2u) {
        fprintf(stderr, "fatal: statement list too long\n");
        exit(EXIT_FAILURE);
    }
    slist->max *= 2u;
    slist->list = (ASTNode**)safe_realloc(slist->list, sizeof(ASTNode*) * slist->max);
}

void append_to_slist(statement_list* slist, ASTNode* new_node) {
    if (slist->count == slist->max) {
        expand_slist(slist);
    }
    slist->list[slist->count++] = new_node;
}

ASTNode* get_from_slist(const statement_list* slist, uint32_t index) {
    if (index >= slist->count) {
        fprintf(stderr, "get_from_slist: index %u out of range (list has %u items)\n",
                (unsigned)index, (unsigned)slist->count);
        return NULL;
    }
    return slist->list[index];
}
