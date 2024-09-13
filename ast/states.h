#ifndef STATES
#define STATES

#include <stdint.h>

#include "../lexer/token.h"
#include "../utility/stack.h"
#include "../utility/data_pool.h"
#include "ast_utility/ast.h"

#include "tables/tables.h"

state_table* acquire_table_from_table_type(table_type type);

typedef enum {
    ERROR,
    SHIFTED,
    REDUCED,
    COMPLETED,
    JUMP
} shift_results;

typedef struct {
    table_type  type;
    uint32_t    state;
    uint32_t    (*table)[WIDTH];
} table_progression;

// Iterates the state tables and shifts/reduces individual tokens
typedef struct{
    uint8_t initiated;
    stack* node_stack;
    stack* progression_stack;
    data_pool* progression_pool;
    data_pool* node_pool;
    table_progression* current;
} table_iterator;

shift_results shift(table_iterator* iterator, token* current_token);

ASTNode* close_iterator(table_iterator* iterator);

void drop_table(table_iterator* iterator);

void push_token_into_ast_node(table_iterator* iterator, token* current_lookahead);

uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead);

void initiate_table(table_iterator*, token*, table_type);

table_iterator* initialize_table_iterator(void);

#endif