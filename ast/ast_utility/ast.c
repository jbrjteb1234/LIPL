#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "../../utility/safe_memory.h"

#define INITIAL_SLIST_SIZE 20

statement_list* create_new_slist(){
    statement_list* new_slist = (statement_list*)safe_malloc(sizeof(statement_list));
    new_slist->list = (ASTNode*)safe_malloc(sizeof(ASTNode)*INITIAL_SLIST_SIZE);
    return new_slist;
}

void append_to_slist(statement_list* working_list, ASTNode* new_node){
    //TODO
}
