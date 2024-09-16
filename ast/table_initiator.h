#ifndef TABLE_INITIATOR
#define TABLE_INITIATOR

#include "tables/tables.h"
#include "../lexer/token.h"

table_type find_tabletype_from_token(token* initiating_token);

#endif