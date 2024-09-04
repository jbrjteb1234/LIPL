#ifndef STATES
#define STATES

#include "../lexer/token.h"
#include "../utility/stack.h"

#define WIDTH 10

typedef enum {
    NUMBERS_TABLE = 0,
    DECL_TABLE = 1
} table_type;

// Iterates the state tables and shifts/reduces individual tokens
typedef struct{
    int state;
    table_type type;
    stack* node_stack;
    int (*table)[WIDTH];
} table_iterator;

void shift(table_iterator* iterator, token* current_token);

void initiate_table(table_iterator*, token*);


table_iterator* initialize_table_iterator();

#endif