#ifndef SLIST_FUNCTIONS
#define SLIST_FUNCTIONS

#include "ast.h"

statement_list* create_new_slist();

void expand_slist(statement_list* slist);

void append_to_slist(statement_list*, ASTNode*);

#endif