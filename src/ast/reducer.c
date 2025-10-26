#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "reducer.h"
#include "ast_utility/slist_functions.h"
#include "../utility/safe_memory.h"
#include "table_management/table.h"
#include "table_management/token_converter.h"

void transfer_specifier(table_iterator* iterator, ASTNode* node){
    safe_memcpy(&(node->node_specifiers), &(iterator->iterator_specifiers), sizeof(specifiers));
    safe_memset(&(iterator->iterator_specifiers), 0, sizeof(specifiers));
}

uint32_t reduce(table_iterator* iterator, uint32_t reduction){

    uint32_t rule = reduction & 0x000fffff;
    stack* node_stack = iterator->node_stack;

    switch(rule){
        /* R0: general binary operation, like 1+1, 2*3, a=4, a.b, etc. */
        case 0: {
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            ASTNode* op  = *(ASTNode**)pop(node_stack);
            ASTNode* lhs = *(ASTNode**)pop(node_stack);
            if (!lhs || !op || !rhs){
                fprintf(stderr, "Reduction 0: stack underflow\n");
                return N;
            }
            op->data.binary_op_node.lhs = lhs;
            op->data.binary_op_node.rhs = rhs;

            push(node_stack, op, false);
            printf("Reduction 0\n");
            return EXPR_STATE;
        }

        /* R1: comma -> build/extend list */
        case 1: {
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            if (!rhs){
                fprintf(stderr, "Reduction 1: missing RHS\n");
                return N;
            }
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            if (lhs && lhs->type != LIST_NODE){
                /* LIST := [lhs, rhs] */
                pop(node_stack);
                ASTNode* list = (ASTNode*)acquire_from_pool(iterator->node_pool);
                safe_memset(list, 0, sizeof(*list));
                list->type = LIST_NODE;
                list->data.list_node = create_new_slist();
                append_to_slist(list->data.list_node, lhs);
                append_to_slist(list->data.list_node, rhs);
                push(node_stack, list, false);
            } else if (lhs) {
                /* extend existing list */
                append_to_slist(lhs->data.list_node, rhs);
            } else {
                fprintf(stderr, "Reduction 1: missing LHS to build list\n");
                return N;
            }
            printf("Reduction 1\n");
            return EXPR_STATE;
        }

        /* R2: function call, after seeing (...) */
        case 2: {
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            if (!rhs) { fprintf(stderr, "Reduction 2: missing RHS\n"); return N; }
            ASTNode* lhs;

            switch(rhs->type){
                case BINARY_OP_NODE:
                case LEAF_NODE:
                    lhs = *(ASTNode**)pop(node_stack);
                    if (!lhs) { fprintf(stderr, "Reduction 2: missing LHS\n"); return N; }
                    lhs->data.function_node.arguments_list  = rhs;
                    lhs->data.function_node.arguments_count = 1;
                    break;
                case LIST_NODE:
                    lhs = *(ASTNode**)pop(node_stack);
                    if (!lhs) { fprintf(stderr, "Reduction 2: missing LHS for list args\n"); return N; }
                    lhs->data.function_node.arguments_list  = rhs;
                    lhs->data.function_node.arguments_count = rhs->data.list_node->index + 1u;
                    break;
                case FUNC_NODE:
                    if (rhs->reduced){
                        /* different func node already reduced, treat as 1-arg call */
                        lhs = *(ASTNode**)pop(node_stack);
                        if (!lhs) { fprintf(stderr, "Reduction 2: missing LHS (reduced func)\n"); return N; }
                        lhs->data.function_node.arguments_list  = rhs;
                        lhs->data.function_node.arguments_count = 1;
                    }else{
                        /* this is our head; zero-arg call so far */
                        lhs = rhs;
                        lhs->data.function_node.arguments_count = 0;
                    }
                    break;
                default:
                    fprintf(stderr, "Reduction 2: invalid RHS node type %d\n", rhs->type);
                    return N;
            }

            lhs->reduced = true;
            push(node_stack, lhs, false);

            printf("Reduction 2\n");
            return EXPR_STATE;
        }

        /* R3: conditional header (if/elif/while) followed by condition expr */
        case 3: {
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            if (!lhs || !rhs){
                fprintf(stderr, "Reduction 3: missing nodes\n");
                return N;
            }
            lhs->data.conditional_block_node.condition = rhs;
            lhs->block_flag = true;
            lhs->reduced = true;

            printf("Reduction 3\n");
            return BLOCK_CONTROL_STATE;
        }

        /* R4: return expr */
        case 4: {
            ASTNode* lhs = *(ASTNode**)peek(node_stack);
            ASTNode* rhs = *(ASTNode**)pop(node_stack);
            if (!lhs || !rhs){
                fprintf(stderr, "Reduction 4: missing nodes\n");
                return N;
            }
            lhs->data.return_block_node.value = rhs;
            printf("Reduction 4\n");
            return EXPR_STATE;
        }

        /* R5: function declaration (func <id>(args) { ... }) */
        case 5 : {
            /* first build the call-shape (R2) to get a FUNC_NODE with args */
            uint32_t st = reduce(iterator, R(2));
            if (st == N){
                fprintf(stderr, "Reduction 5: failed to reduce to FUNC head\n");
                return N;
            }

            ASTNode* fn = *(ASTNode**)peek(node_stack);
            if (!fn){ fprintf(stderr, "Reduction 5: missing function node\n"); return N; }

            fn->value.func_node_value = FUNC_DEC_NODE;
            fn->block_flag = true;

            transfer_specifier(iterator, fn);

            printf("Reduction 5 (func decl)\n");
            return BLOCK_CONTROL_STATE;
        }

        /* R6: variable declaration  (var <id> = <expr>;) */
        case 6 : {
            /* build assignment binary op first */
            uint32_t st = reduce(iterator, R(0));
            if (st == N){
                fprintf(stderr, "Reduction 6: failed to reduce to assignment\n");
                return N;
            }

            ASTNode* dec_node = *(ASTNode**)peek(node_stack);
            if (!dec_node){ fprintf(stderr, "Reduction 6: missing decl node\n"); return N; }

            dec_node->value.leaf_node_value = DEC_NODE;
            transfer_specifier(iterator, dec_node);

            printf("Reduction 6 (var decl)\n");
            return EXPR_STATE;
        }
    }

    fprintf(stderr, "Invalid reduction rule %u\n", rule);
    return N;
}