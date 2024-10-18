#include "ast.h"
#include "../tables/tables.h"

uint32_t open_expression_parentheses(table_iterator* iterator, uint32_t new_state);

void return_to_table(table_iterator* iterator, uint32_t new_state);

