#ifndef TABLES
#define TABLES

#include <stdint.h>

typedef enum {
    NONE = -1,
    NUMBERS_TABLE = 0,
    ASSIGNMENT_TABLE = 1
} table_type;

#define WIDTH 10
#define HEIGHT 10

//converts int (jump rule)
#define jump_mask 0x10000000
#define new_state_shift_count 20
#define J(table_to_jump, new_state) ((jump_mask | (table_to_jump&0x000fffff)) | (new_state << new_state_shift_count))

#define reduction_mask  0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(reduction_rule,return_state) ((reduction_mask | reduction_rule) | (return_state<<reduction_return_state_shift_count))

#define N 0x30000000
#define A 0x40000000

typedef uint32_t state_table[HEIGHT][WIDTH];

state_table* get_numbers_table(void);

state_table* get_assignment_table(void);

#endif