#ifndef REDUCER
#define REDUCER

#define N -1
#define A -2

#define reduction_mask  0x20000000  //indicates that this is a reduction call
#define reduction_return_state_shift_count 20
#define R(X,R) ((reduction_mask | X) | R<<reduction_return_state_shift_count)

#include "../utility/stack.h"
#include "ast.h"

int reduce(stack* ast_stack, int reduction);

#endif