#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "reducer.h"
#include "ast_utility/ast.h"

uint32_t reduce(stack* node_stack, uint32_t reduction){

    uint32_t rule = reduction & 0x000fffff;
    uint32_t return_state = (reduction & 0x0ff00000) >> reduction_return_state_shift_count;
    
    switch(rule){
        //basic binary operation, like 1+1 or 2*3
        //numbers table reduction
        case 0: {
            ASTNode* rhs = pop(node_stack);
            ASTNode* operator = pop(node_stack);
            ASTNode* lhs = pop(node_stack);

            operator->data.binary_op_node.lhs = lhs;
            operator->data.binary_op_node.rhs = rhs;
            operator->data.binary_op_node.type = operator->token->token_value.operator_token_value;

            push(node_stack, operator);
            printf("Reduction 0, returning to state %u\n", return_state);

            return return_state;
        }

        case 1: {
            ASTNode* value = pop(node_stack);
            ASTNode* assigner = pop(node_stack);
            ASTNode* identifier = pop(node_stack);

            assigner->data.decl_node.identifier = identifier;
            assigner->data.decl_node.value = value;

            printf("Reduction 1, returning to state %u\n", return_state);

            return return_state;

            break;
        }
    }

    //invalid rule providede
    perror("Invalid reduction rule provided\n");
    return N;
}