#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "reducer.h"
#include "ast_utility/slist_functions.h"

uint32_t reduce(table_iterator* iterator, uint32_t reduction){

    uint32_t rule = reduction & 0x000fffff;
    uint32_t return_state = (reduction & 0x0ff00000) >> reduction_return_state_shift_count;
    stack* node_stack = iterator->node_stack;
    
    switch(rule){
        //general binary operation, like 1+1 or 2*3, a=4, 4.4
        case 0: {
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            ASTNode* operator = *(ASTNode**)pop(node_stack);
            ASTNode* lhs = *(ASTNode**)pop(node_stack);

            operator->data.binary_op_node.lhs = lhs;
            operator->data.binary_op_node.rhs = rhs;

            push(node_stack, operator, false);
            printf("Reduction 0, returning to state %u\n", return_state);

            return return_state;
        }
        

        //commma reduction
        //produce an ast node with a list of expressions
        case 1: {
            //ITEM, ITEM -> LIST [ITEM, ITEM]
            //or
            //LIST [ITEM, ITEM], ITEM -> LIST [ITEM, ITEM, ITEM]
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            if(rhs->type == LIST_NODE){
                perror("Tried to append NULL to list node\n");
                return N;
            }
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            if(lhs->type != LIST_NODE){
                pop(node_stack);
                ASTNode* new_expr_list = acquire_from_pool(iterator->node_pool);
                new_expr_list->type = LIST_NODE;
                new_expr_list->data.list_node = create_new_slist();
                append_to_slist(new_expr_list->data.list_node, lhs);
                append_to_slist(new_expr_list->data.list_node, rhs);
                push(node_stack, new_expr_list, false);
            }else{
                append_to_slist(lhs->data.list_node, rhs);
            }
            
            printf("Reduction 1, returning to state %u\n", return_state);
            return return_state;
        }
    }

    //invalid rule providede
    perror("Invalid reduction rule provided\n");
    return N;
}