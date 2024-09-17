#ifndef TABLE_INITIATOR
#define TABLE_INITIATOR

#include "tables/tables.h"
#include "../lexer/token.h"

void initiate_statement(token** initiating_token, table_iterator* iterator);

#endif