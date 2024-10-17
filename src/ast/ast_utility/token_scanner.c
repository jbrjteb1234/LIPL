#include <stdio.h>
#include <stdlib.h>
#include "token_scanner.h"
#include "ast.h"

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

token* lookahead(token** scan_token){
    if((*scan_token)->next != NULL){
        return (*scan_token)->next;
    }else{
        //perror("Tried to look ahead past end of tokenstream\n");
        return NULL;
    }
}

/** converts token directly from tokenstream into an AST node
 *  adds AST node to the stack for reduction
 */
ASTNode* push_token_into_ast_node(table_iterator* iterator, token** current_lookahead, bool auto_assign){

    ASTNode* new_ast_node = acquire_from_pool(iterator->node_pool);
    new_ast_node->block_flag=0;

    if(auto_assign){
        if (current_lookahead == NULL){
            perror("Tried to auto assign without lookahead token\n");
            return NULL;
        }
        assign_ast_node_type(new_ast_node, current_lookahead);
    }else{
        new_ast_node->type = NONE_AST_TYPE;
    }


    push(iterator->node_stack, new_ast_node, false);
    return new_ast_node;
}