#ifndef REDUCER
#define REDUCER

#define N -1
#define A -2
#define R(A) (-(A)-3)

#include "../utility/stack.h"
#include "ast.h"

int reduce(stack* ast_stack, int reduction);

#endif