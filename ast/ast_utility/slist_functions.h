#ifndef SLIST_FUNCTIONS
#define SLIST_FUNCTIONS

#include "ast.h"

statement_list* create_new_slist();

void append_to_slist(statement_list*, ASTNode*);

#endif