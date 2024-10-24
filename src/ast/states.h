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
    HOLD,
    ADVANCE,
    OPEN_BLOCK,
    FINISH,
    NOT_INITIATED,
} shift_results;

shift_results shift(table_iterator* iterator, token** current_token);

ASTNode* close_iterator(table_iterator* iterator);

state_table* acquire_table_from_table_type(table_type type);

uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead);

table_iterator* initialize_table_iterator(statement_list* global_slist);

#endif