#ifndef REDUCER
#define REDUCER

#include <stdint.h>

#include "../utility/stack.h"
#include "ast_utility/ast.h"
#include "table_management/table.h"

uint32_t reduce(table_iterator* iterator, uint32_t reduction);

#endif