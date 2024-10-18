#ifndef TABLE_INITIATOR
#define TABLE_INITIATOR

#include "tables.h"
#include "../../lexer/token.h"

void initiate_statement(token** initiating_token, table_iterator* iterator);

void initiate_table(table_iterator* iterator, token** initiating_token, uint32_t state_to_save);

#endif