#ifndef TOKEN_CONVERTER
#define TOKEN_CONVERTER

#include "../ast_utility/ast.h"
#include "table.h"

/* Column layout used by state_table (keep in sync with table.c comment) */
typedef enum {
    COL_EXPR            = 0,
    COL_ADD_SUB         = 1,   /* + - */
    COL_MUL_DIV         = 2,   /* * / ^ _ */
    COL_EOS             = 3,   /* ; */
    COL_DOT             = 4,   /* . */
    COL_ASSIGN          = 5,   /* = */
    COL_COMMA           = 6,   /* , */
    COL_OPEN_BRACKET    = 7,   /* ( */
    COL_CLOSE_BRACKET   = 8,   /* ) */
    COL_COMP            = 9,   /* < > <= >= == (and != if/when added) */
    COL_OPEN_CBRACKET   = 10,  /* { */
    COL_CLOSE_CBRACKET  = 11,  /* } */
    COL_FUNC            = 12,  /* identifier followed by '(' => function call head */
    COL_VAR             = 13,  /* var */
    COL_IF_WH_ELI       = 14,  /* if / while / elif */
    COL_ELSE            = 15,  /* else */
    COL_RETURN          = 16,  /* return */
    COL_FDEC            = 17,  /* func (function declaration keyword) */
    COL_GLOBAL          = 18,  /* global specifier (applies to next decl) */
    COL_CONST           = 19   /* const  specifier (applies to next decl) */
} parse_column;

/* convert current lookahead token -> column index for the LR table,
   and stage an AST node (iterator->new_node_buffer) when appropriate.

   Returns: 0..WIDTH-1 (never an out-of-range value).
*/
uint32_t convert_token(table_iterator* it, token** lookahead);

/* pop one saved state into iterator->state (used by states.c).
   Implemented in ast_utility/routines.c. */
void return_to_previous_state(table_iterator* it);

/* Map a token to a parse column without creating nodes (exported for tests) */
uint32_t token_to_column(const token* t, const token* next);

#endif /* TOKEN_CONVERTER */
