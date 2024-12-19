#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "routines.h"


void return_to_previous_state(table_iterator* iterator){
    if(iterator->return_stack->top == -1){
        perror("Tried to return to previous state without a previous state\n");
        return;
    }
    uint32_t new_state = *(uint32_t*)pop(iterator->return_stack);
    iterator->state = new_state;
}

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

void attach_to_previous_conditional_block(table_iterator* iterator, ASTNode* new_block_node){
    ASTNode* last_statement = get_from_slist(iterator->working_list, iterator->working_list->index);
    if(last_statement->type != CONDITIONAL_BLOCK_NODE || last_statement->value.conditional_block_node_value == ELSE_NODE){
        perror("\nTried to attach to unattachable block\n");
        return;
    }

    last_statement->data.conditional_block_node.alternate = new_block_node;
}

void transfer_node_buffer(table_iterator* iterator){
    if(iterator->new_node_buffer_set_flag){
        push(iterator->node_stack, iterator->new_node_buffer, false);
    }
    iterator->new_node_buffer_set_flag = false;
}