#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "routines.h"

/* Pop one saved state into iterator->state if available.
 * If there is no previous state, this is a no-op.
 */
void return_to_previous_state(table_iterator* iterator) {
    if (!iterator || !iterator->return_stack) {
        return;
    }

    uint32_t* s = (uint32_t*)pop(iterator->return_stack);
    if (!s) {
        /* Nothing to return to; leave iterator->state unchanged. */
        return;
    }

    iterator->state = *s;
}

/* Move to next token in token stream (if possible). */
void advance_token(token** scan_token) {
    if (scan_token && *scan_token && (*scan_token)->next != NULL) {
        *scan_token = (token*)(*scan_token)->next;
    } else {
        /* Silent no-op if already at end or NULL. */
    }
}

/* Peek one token ahead without advancing the pointer. */
token* lookahead(token** scan_token) {
    if (scan_token && *scan_token) {
        return (token*)(*scan_token)->next;
    }
    return NULL;
}
