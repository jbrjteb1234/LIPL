#ifndef SLIST_FUNCTIONS
#define SLIST_FUNCTIONS

#include "ast.h"

statement_list* create_new_slist(void);

void expand_slist(statement_list* slist);

void append_to_slist(statement_list*, ASTNode*);

ASTNode* get_from_slist(statement_list* slist, uint32_t index)

#endif