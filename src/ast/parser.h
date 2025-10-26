#ifndef PARSER
#define PARSER

#include "ast_utility/ast.h"
#include "table_management/table.h"

/* Parse a token stream and return the root statement list.
   Ownership:
   - The returned AST nodes live in a pool recorded in root->backing_pool.
   - Call destroy_ast(root) when done. */
statement_list* parse(token** token_head);

/* Internal helper used by the state machine; exposed for tests. */
void transfer_node_buffer(table_iterator* iterator);

/* Destroy the AST forest rooted at 'root':
   - Frees all statement_list containers (and their arrays) recursively.
   - Shuts down the AST node pool exactly once (root->backing_pool). */
void destroy_ast(statement_list* root);

#endif /* PARSER */