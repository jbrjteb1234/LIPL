#ifndef TABLES
#define TABLES

#include <stdint.h>
#include "../../utility/data_pool.h"
#include "../../utility/stack.h"
#include "../ast_utility/ast.h"
#include "../ast_utility/slist_functions.h"

#define WIDTH 50
#define HEIGHT 50

#define INIT_STATE 0
#define EXPR_STATE 1
#define BLOCK_CONTROL_STATE 14

#define EXPR_INDEX 0

#define FUNC_INDEX 12

#define general_mask (uint32_t) 0xf0000000

#define reduction_mask  (uint32_t) 0x20000000 
#define R(reduction_rule) (reduction_mask | reduction_rule)

#define save_mask (uint32_t) 0x30000000
#define S(save_new_state) (save_mask | save_new_state)

#define O (uint32_t) 0x50000000

#define C (uint32_t) 0x60000000

#define OB (uint32_t) 0x70000000
#define CB (uint32_t) 0x80000000

#define N (uint32_t) 0x90000000
#define A (uint32_t) 0xa0000000

#define save_with_vs_mask (uint32_t) 0xb0000000
#define SV(save_new_state) (save_with_vs_mask | save_new_state)

// Iterates the state tables and shifts/reduces individual tokens
typedef struct{
    uint32_t state;

    statement_list* working_list;

    bool new_node_buffer_set_flag;
    ASTNode* new_node_buffer;

    stack* node_stack;
    stack* return_stack;
    data_pool* node_pool;

    specifiers iterator_specifiers;

    uint32_t    (*table)[WIDTH];

} table_iterator;

typedef uint32_t state_table[HEIGHT][WIDTH];

state_table* get_state_table(void);

void apply_virtual_shift(table_iterator* iterator);

#endif