#include <stdlib.h>
#include <stdio.h>
#include "reducer.h"
#include "ast.h"

int reduce(stack* node_stack, int reduction){
    switch(reduction){
        //basic binary operation, like 1+1 or 2*3
        //numbers table reduction
        case R(0):
            ASTNode* rhs = pop(node_stack);
            ASTNode* operator = pop(node_stack);
            ASTNode* lhs = pop(node_stack);

            operator->data.binary_op_node.lhs = lhs;
            operator->data.binary_op_node.rhs = rhs;
            operator->data.binary_op_node.type = operator->token->token_value.operator_token_value;

            push(node_stack, operator);

            return 0;

        case R(1):

            break;

    }
}