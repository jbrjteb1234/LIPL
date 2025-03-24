#ifndef STATES
#define STATES

#include <stdint.h>

#include "../lexer/token.h"
#include "../utility/stack.h"
#include "../utility/data_pool.h"
#include "ast_utility/ast.h"

#include "table_management/table.h"

typedef enum {
    ERROR,
    HOLD,
    ADVANCE,
    OPEN_BLOCK,
    CLOSE_BLOCK,
    FINISH,
} shift_results;

shift_results shift(table_iterator* iterator, token** current_token);

ASTNode* close_iterator(table_iterator* iterator);

table_iterator* initialize_table_iterator(statement_list* global_slist);

#endif