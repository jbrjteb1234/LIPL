#ifndef STATES
#define STATES

#include <stdint.h>

#include "../lexer/token.h"
#include "../utility/stack.h"
#include "../utility/data_pool.h"
#include "ast.h"

#define WIDTH 10

//converts int (jump rule)
#define jump_mask 0x10000000
#define J(x) (jump_mask | x)

typedef enum {
    NUMBERS_TABLE = 0,
    DECL_TABLE = 1
} table_type;

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
    table_progression* current;
} table_iterator;

bool shift(table_iterator* iterator, token* current_token);

ASTNode* close_iterator(table_iterator* iterator, statement_list* working_list);

void initiate_table(table_iterator*, token*);


table_iterator* initialize_table_iterator();

#endif