#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "reducer.h"
#include "ast.h"

int reduce(stack* node_stack, int reduction){

    uint32_t rule = reduction & 0x000fffff;
    uint32_t return_state = (reduction & 0x0ff00000) >> reduction_return_state_shift_count;

    switch(rule){
        //basic binary operation, like 1+1 or 2*3
        //numbers table reduction
        case 0:
            ASTNode* rhs = pop(node_stack);
            ASTNode* operator = pop(node_stack);
            ASTNode* lhs = pop(node_stack);

            operator->data.binary_op_node.lhs = lhs;
            operator->data.binary_op_node.rhs = rhs;
            operator->data.binary_op_node.type = operator->token->token_value.operator_token_value;

            push(node_stack, operator);
            printf("Reduction 0, returning to state %u\n", return_state);

            return return_state;

        case 1:

            break;

    }
}