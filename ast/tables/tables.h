#ifndef TABLES
#define TABLES

#include <stdint.h>
#include "../../utility/data_pool.h"
#include "../../utility/stack.h"

#define VAR_ENTRY 1;

#define NUM_ENTRY 0;

typedef enum {
    NONE = -1,
    EXPR_TABLE = 0,
    ASSIGNMENT_TABLE = 1,
    RESERVED_TABLE = 2,
    STRING_TABLE = 3,
} table_type;

#define WIDTH 10
#define HEIGHT 10

#define general_mask 0xf0000000

//converts int (jump rule)
#define jump_mask 0x10000000
#define new_state_shift_count 20
#define J(table_to_jump, new_state) ((jump_mask | (table_to_jump&0x000fffff)) | (new_state << new_state_shift_count))

#define reduction_mask  0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(reduction_rule,return_state) ((reduction_mask | reduction_rule) | (return_state<<reduction_return_state_shift_count))

#define save_mask 0x30000000
#define S(new_state) (save_mask | new_state)

#define N 0x40000000
#define A 0x50000000

typedef struct {
    table_type  type;
    uint32_t    state;
    stack*      return_stack;
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

typedef uint32_t state_table[HEIGHT][WIDTH];

state_table* get_expr_table(void);

state_table* get_reserved_table(void);

#endif