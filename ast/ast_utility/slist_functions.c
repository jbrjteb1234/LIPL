#include <stdio.h>
#include <stdlib.h>
#include "slist_functions.h"
#include "../../utility/safe_memory.h"

#define INITIAL_SLIST_SIZE 20

statement_list* create_new_slist(void){
    statement_list* new_slist = (statement_list*)safe_malloc( (size_t) sizeof(statement_list));
    new_slist->list = (ASTNode**)safe_malloc( (size_t) sizeof(ASTNode*)*INITIAL_SLIST_SIZE);
    new_slist->index = 0;
    new_slist->max = INITIAL_SLIST_SIZE;
    return new_slist;
}

void expand_slist(statement_list* slist){
    slist->max*=2;
    slist->list = (ASTNode**)safe_realloc(slist->list, (size_t) sizeof(ASTNode*)*slist->max);
}

void append_to_slist(statement_list* slist, ASTNode* new_node){
    if(slist->index == slist->max){
        expand_slist(slist);
    }
    slist->list[slist->index] = new_node;
}