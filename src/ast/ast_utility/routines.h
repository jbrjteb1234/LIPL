#ifndef ROUTINES_H
#define ROUTINES_H

#include "ast.h"
#include "../table_management/table.h"

/* Pop a previously saved parser state into iterator->state (if any). */
void return_to_previous_state(table_iterator* iterator);

/* Advance the token pointer by one, guarding against NULL. */
void advance_token(token** scan_token);

/* Look one token ahead; may return NULL if at end or on NULL input. */
token* lookahead(token** scan_token);

#endif /* ROUTINES_H */
