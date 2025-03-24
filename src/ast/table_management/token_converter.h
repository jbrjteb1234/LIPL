#ifndef TOKEN_CONVERTER
#define TOKEN_CONVERTER

#include "../ast_utility/ast.h"
#include "table.h"

uint32_t convert_token(table_iterator* iterator, token** current_lookahead_addr);

#endif