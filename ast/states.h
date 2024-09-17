#ifndef STATES
#define STATES

#include <stdint.h>

#include "../lexer/token.h"
#include "../utility/stack.h"
#include "../utility/data_pool.h"
#include "ast_utility/ast.h"

#include "tables/tables.h"

typedef enum {
    ERROR,
    SHIFTED,
    REDUCED,
    COMPLETED,
    JUMP
} shift_results;

shift_results shift(table_iterator* iterator, token** current_token);

ASTNode* close_iterator(table_iterator* iterator);

state_table* acquire_table_from_table_type(table_type type);

void drop_table(table_iterator* iterator);

uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead);

void initiate_table(table_iterator*, token**, table_type);

table_iterator* initialize_table_iterator(void);

#endif