#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "slist_functions.h"
#include "../../utility/safe_memory.h"

#define INITIAL_SLIST_SIZE 20

/* Sentinel: no elements in the list.
 * Invariant:
 *   - When non-empty, index is the last valid element (0 .. max-1).
 *   - When empty, index == SLIST_EMPTY_INDEX.
 */
#define SLIST_EMPTY_INDEX 0xFFFFFFFFu

statement_list* create_new_slist(void) {
    statement_list* new_slist = (statement_list*)safe_malloc(sizeof(statement_list));

    new_slist->list = (ASTNode**)safe_malloc(sizeof(ASTNode*) * INITIAL_SLIST_SIZE);
    new_slist->max  = INITIAL_SLIST_SIZE;

    /* Empty list: no valid elements yet. */
    new_slist->index = SLIST_EMPTY_INDEX;

    /* Backing pool is set later (for root/block lists) by the parser. */
    new_slist->backing_pool = NULL;

    return new_slist;
}

void expand_slist(statement_list* slist) {
    if (!slist) return;

    slist->max *= 2u;
    slist->list = (ASTNode**)safe_realloc(
        slist->list,
        (size_t)sizeof(ASTNode*) * slist->max
    );
}

/* Append a node to the end of the list, growing if necessary.
 *
 * Uses the invariant:
 *   - index == SLIST_EMPTY_INDEX  => next element goes at 0
 *   - otherwise                  => next element goes at index + 1
 */
void append_to_slist(statement_list* slist, ASTNode* new_node) {
    if (!slist || !new_node) {
        return;
    }

    uint32_t next_index;

    if (slist->index == SLIST_EMPTY_INDEX) {
        /* First element in an empty list. */
        next_index = 0u;
    } else {
        next_index = slist->index + 1u;
    }

    /* Grow if the next write would exceed capacity. */
    if (next_index >= slist->max) {
        expand_slist(slist);
    }

    slist->index = next_index;
    slist->list[slist->index] = new_node;
}

/* Safe indexed access. */
ASTNode* get_from_slist(statement_list* slist, uint32_t index) {
    if (!slist) {
        return NULL;
    }

    /* Empty list or out-of-range access. */
    if (slist->index == SLIST_EMPTY_INDEX || index > slist->index) {
        perror("Tried to access index out of bounds in statement list\n");
        return NULL;
    }

    return slist->list[index];
}
