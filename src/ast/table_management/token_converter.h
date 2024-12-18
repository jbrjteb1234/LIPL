#ifndef TOKEN_CONVERTER
#define TOKEN_CONVERTER

#include "../ast_utility/ast.h"
#include "table.h"

#define VAR_INDEX 0

#define ID_INDEX 1

#define FUNC_INDEX 13

uint32_t convert_token(table_iterator* iterator, token** current_lookahead_addr);

#endif