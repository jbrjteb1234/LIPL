#include <stdio.h>
#include <stdlib.h>
#include "token_scanner.h"

/** Move to next token in tokenstream
 * 
 */
void advance_token(token** scan_token){
    if((*scan_token)->next != NULL){
        (*scan_token) = (*scan_token)->next;
    }else{
        //perror("Tried to advance past end of tokenstream\n");
    }
}

/** converts token directly from tokenstream into an AST node
 *  adds AST node to the stack for reduction
 */
void push_token_into_ast_node(table_iterator* iterator, token** current_lookahead){
    if (current_lookahead == NULL){
        return;
    }

    token* new_token_pointer;
    new_token_pointer = *current_lookahead;
    ASTNode* new_ast_node = acquire_from_pool(iterator->node_pool);
    new_ast_node->token = new_token_pointer;

    //if it is a leaf node  then we can transfer data from token to node immedietly
    if((*current_lookahead)->leaf == 1){
        new_ast_node->type = LEAF_NODE;
        if((*current_lookahead)->token_type == IDENTIFIER){
            new_ast_node->value.leaf_node_value = ID_NODE;
            new_ast_node->data.value_node.identifier = (*current_lookahead)->token_value.identifier_token_value;
        }else{
            new_ast_node->value.leaf_node_value = INT_NODE;
            new_ast_node->data.value_node.value = (*current_lookahead)->token_value.variable_value;
        }
    }

    push(iterator->node_stack, new_ast_node, false);
}