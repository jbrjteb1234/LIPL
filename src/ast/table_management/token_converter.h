#ifndef TOKEN_CONVERTER
#define TOKEN_CONVERTER

#include "../ast_utility/ast.h"
#include "table.h"

typedef enum {
    C_ERROR=-1,
    C_ADVANCE=-2,
    C_ADVANCE_ADD_ALTERNATE=-3,
} token_conversion_results;

token_conversion_results convert_token(table_iterator* iterator, token** current_lookahead_addr);

#endif