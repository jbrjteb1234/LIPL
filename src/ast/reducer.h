#ifndef REDUCER
#define REDUCER

#include <stdint.h>

#include "../utility/stack.h"
#include "ast_utility/ast.h"
#include "tables/tables.h"

uint32_t reduce(table_iterator* iterator, uint32_t reduction);

#endif