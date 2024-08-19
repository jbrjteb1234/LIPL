#ifndef STATES
#define STATES

#include "../lexer/token.h"
#include "../utility/stack.h"

#define N -1
#define A -2
#define R(A) (-(A)-3)

extern const int numbers_table[][4];

extern const int decl_table[][6];

typedef enum {
    NUMBERS_TABLE,
    DECL_TABLE
} table_type;


typedef struct{
    int** table;
    int state;
    table_type type;
    stack* token_stack;
} table_iterator;

void advance_state(table_iterator*);

void initiate_table(table_iterator*, token*);

table_iterator* initialize_table_iterator();

#endif