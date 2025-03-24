#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "reducer.h"
#include "ast_utility/slist_functions.h"
#include "table_management/table.h"

uint32_t reduce(table_iterator* iterator, uint32_t reduction){

    uint32_t rule = reduction & 0x000fffff;
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
            printf("Reduction 0\n");

            return EXPR_STATE;
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
            if(lhs != NULL && lhs->type != LIST_NODE){
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
            
            printf("Reduction 1\n");
            return EXPR_STATE;
        }

        case 2: {
            //reductions for function call
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            ASTNode* lhs;

            switch(rhs->type){
                case BINARY_OP_NODE:
                case LEAF_NODE:
                    lhs = *(ASTNode**)pop(node_stack);
                    lhs->data.function_node.arguments_list = rhs;
                    lhs->data.function_node.arguments_count = 1;
                    break;
                case LIST_NODE:
                    lhs = *(ASTNode**)pop(node_stack);
                    lhs->data.function_node.arguments_list = rhs;
                    lhs->data.function_node.arguments_count = rhs->data.list_node->index+1;
                    break;
                case FUNC_NODE:
                    if(rhs->reduced){
                        //if rhs is reduced, then it is a different func node, not the one we are reducing
                        lhs = *(ASTNode**)pop(node_stack);
                        lhs->data.function_node.arguments_list = rhs;
                        lhs->data.function_node.arguments_count = 1;
                    }else{
                        //if rhs is not reduced, then it is the func node we are currently reducing
                        lhs = rhs;
                        lhs->data.function_node.arguments_count = 0;
                    }

                    break;
                default:
                    perror("Invalid node for reduction rule 2\n");
                    return N;
            }

            lhs->reduced = true;

            push(node_stack, lhs, false);
            
            printf("Reduction 2\n");
            return EXPR_STATE;
        }

        case 3: {
            //reduction for conditional statements (if, elif, while)
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            ASTNode* rhs = *(ASTNode**)pop(node_stack);

            lhs->data.conditional_block_node.condition = rhs;
            lhs->data.conditional_block_node.alternate = NULL;

            lhs->block_flag = true;
            lhs->reduced = true;

            printf("Reduction 3\n");
            return BLOCK_CONTROL_STATE;
        }

        case 4: {
            //reduction for return
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            ASTNode* rhs = *(ASTNode**)pop(node_stack);

            lhs->data.return_block_node.value = rhs;

            printf("Reduction 4\n");
            return EXPR_STATE;
        }

        case 5: {
            //reduction for function decleration

            reduce(iterator, R(2));

            ASTNode* func_node = *(ASTNode**)peek(node_stack);

            func_node->value.func_node_value = FUNC_DEC_NODE; 
            func_node->block_flag = 1; 

            printf("Reduction 2 modified by reduction 5\n");

            return BLOCK_CONTROL_STATE;

        }

        case 6 : {
            //reduction for variable decleration

            reduce(iterator, R(0));

            ASTNode* dec_node = *(ASTNode**)peek(node_stack);

            dec_node->value.leaf_node_value = DEC_NODE;

            printf("Reudction 0 modified by reduction 6\n");

            return EXPR_STATE;
        }
    }

    //invalid rule providede
    perror("Invalid reduction rule provided\n");
    return N;
}