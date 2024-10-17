#ifndef TABLES
#define TABLES

#include <stdint.h>
#include "../../utility/data_pool.h"
#include "../../utility/stack.h"

#define NUM_ENTRY 0;

typedef enum {
    NONE_TABLE = -1,
    EXPR_TABLE = 0,
    RESERVED_TABLE = 1,
} table_type;

#define WIDTH 20
#define HEIGHT 30

#define general_mask 0xf0000000

//converts int (jump rule)
#define jump_mask 0x10000000
#define new_state_shift_count 20
#define J(table_to_jump, jump_new_state) ((jump_mask | (table_to_jump&0x000fffff)) | (jump_new_state << new_state_shift_count))

#define reduction_mask  0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(reduction_rule,return_state) ((reduction_mask | reduction_rule) | (return_state<<reduction_return_state_shift_count))

#define save_mask 0x30000000
#define S(save_new_state) (save_mask | save_new_state)

#define sna_mask (0x40000000)
#define SNA(sna_new_state) (sna_mask | sna_new_state)

#define open_parentheses 0x50000000
#define open_parentheses_state_shift_count 20
#define O(open_parentheses_state, state_after_close) ((open_parentheses | state_after_close) | (open_parentheses_state<<open_parentheses_state_shift_count))

#define EXPR_OPENPAREN_STATE 17
#define FCALL_EXPR_STATE 18

#define C 0x60000000

#define N 0x70000000
#define A 0x80000000

// Iterates the state tables and shifts/reduces individual tokens
typedef struct{
    uint32_t state;

    uint8_t initiated;
    stack* node_stack;
    stack* parentheses_stack;
    data_pool* node_pool;

    stack* return_stack;

    table_type  type;
    uint32_t    (*table)[WIDTH];

    uint32_t    (*reserved_table)[WIDTH];
    uint32_t    (*expr_table)[WIDTH];
} table_iterator;

typedef uint32_t state_table[HEIGHT][WIDTH];

state_table* get_expr_table(void);

state_table* get_reserved_table(void);

#endif