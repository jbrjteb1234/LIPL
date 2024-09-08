#ifndef REDUCER
#define REDUCER

#define N 0x30000000
#define A 0x40000000

#define reduction_mask  0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(reduction_rule,return_state) ((reduction_mask | reduction_rule) | return_state<<reduction_return_state_shift_count)

#include "../utility/stack.h"
#include "ast_utility/ast.h"

int reduce(stack* ast_stack, int reduction);

#endif