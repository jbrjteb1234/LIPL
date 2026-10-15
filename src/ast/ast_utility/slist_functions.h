#ifndef LIPL_AST_SLIST_FUNCTIONS_H
#define LIPL_AST_SLIST_FUNCTIONS_H

#include "ast.h"

/* Growable array of AST node pointers. Lists inside a tree should be made
   with new_ast_list() so the tree owns them; these are the raw operations. */
statement_list* create_new_slist(void);

void destroy_slist(statement_list* slist);

void append_to_slist(statement_list* slist, ASTNode* new_node);

/* Returns NULL (and reports it) for an out-of-range index. */
ASTNode* get_from_slist(const statement_list* slist, uint32_t index);

#endif /* LIPL_AST_SLIST_FUNCTIONS_H */
