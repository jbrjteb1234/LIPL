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

#define EXPR_INDEX 0

#define FUNC_INDEX 12

#define general_mask (uint32_t) 0xf0000000

#define reduction_mask  (uint32_t) 0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(reduction_rule,return_state) ((reduction_mask | reduction_rule) | (return_state<<reduction_return_state_shift_count))

#define save_mask (uint32_t) 0x30000000
#define S(save_new_state) (save_mask | save_new_state)

#define O (uint32_t) 0x50000000

#define C (uint32_t) 0x60000000

#define OB (uint32_t) 0x70000000
#define CB (uint32_t) 0x80000000

#define N (uint32_t) 0x90000000
#define A (uint32_t) 0xa0000000

#define NI (uint32_t) 0xb0000000

//sets item's specifiers field 
#define set_specifiers(item, byte) ((iterator)->specifiers |= (1 << (byte)))
#define read_specifiers_field(item, res_token_val) ((iterator)->specifiers & (1 << (res_token_val)))

// Iterates the state tables and shifts/reduces individual tokens
typedef struct{
    uint32_t state;
    uint16_t specifiers;

    statement_list* working_list;

    bool new_node_buffer_set_flag;
    ASTNode* new_node_buffer;

    stack* node_stack;
    stack* return_stack;
    data_pool* node_pool;

    uint32_t    (*table)[WIDTH];

} table_iterator;

typedef uint32_t state_table[HEIGHT][WIDTH];

state_table* get_state_table(void);

void apply_virtual_state(table_iterator* iterator);

#endif