#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "states.h"
#include "../utility/safe_memory.h"
#include "../lexer/token.h"
#include "../grammar/grammar.h"
#include "../utility/stack.h"
#include "ast_utility/ast.h"
#include "reducer.h"
#include "tables/table_initiator.h"
#include "ast_utility/token_scanner.h"
#include "ast_utility/routines.h"
#include "tables/token_to_index.h"

/** interprets next token in the stream - 
 *  iterates state
 */
shift_results shift(table_iterator* iterator, token** current_lookahead){
    if (iterator->initiated == 0 || iterator->table == NULL){
        if( (*current_lookahead)->token_type == DELIMITER && (*current_lookahead)->token_value.delimiter_token_value == CLOSE_CBRACKET){
            return CLOSE_BLOCK;
        }
        return NOT_INITIATED;
    }

    //finds the index in the table that the token points to
    uint32_t new_index = convert_token_to_index(iterator, *current_lookahead);
    uint32_t new_state;
    //if new_index is N, then an unrecognised symbol has appeared
    switch(new_index){
        //acquire the new state
        case(N):
            //error - unrecognised symbol
            perror("Unrecognised symbol\n");
            return ERROR;
            break;
        //reserved word - set the specifiers register and ADVANCE
        case(C):
            return ADVANCE;
        default:
            new_state = iterator->table[iterator->state][new_index];
    }

    printf("The next state is %d, pointed to by index %d, on current state %d\n",new_state, new_index, iterator->state);
    uint32_t new_state_type = new_state & general_mask;
    //finished parsing statement, passed to the iterator above
    switch(new_state_type){
        case(A): {
            //firstly, check if theres a saved state to return to
            if(iterator->return_stack->top > -1){
                return_to_previous_state(iterator);
                return HOLD;
            //if no saved states and no other tables to return to, then the statement is completed
            }else{
                return FINISH;
            }
        
        }case(N): {
            perror("Unexpected symbol\n");
            return ERROR;
            //error

        }case(reduction_mask): {
            //apply reduction rule and then push new token
            new_state = reduce(iterator, new_state);

            uint32_t* return_state = (uint32_t*)peek(iterator->return_stack);

            if(return_state != NULL && *return_state != C){
                //check if there is states to return to
                printf("Reduced, now returning to %u\n", *return_state);
                return_to_previous_state(iterator);
                return HOLD;
            }

            //the reduction rule gives a new state to return to, then call again to push the lookahead
            if (new_state != N){
                iterator->state = new_state;
                return HOLD;
            }
            perror("Invalid reduction\n");
            return ERROR;

        }case(jump_mask): {
            
            //parse the jump state from the table
            new_state = ( (new_state & 0x0ff00000) >> new_state_shift_count);
            //sets the current table to the state after parsing the table jumped to

            //firstly check the token after the current lookahead - if its a delimiter, we dont need to iterate the FSM - continue on current table to delimiter
            if((*current_lookahead)->next != NULL && (*current_lookahead)->next->token_type == DELIMITER && (*current_lookahead)->next->token_value.delimiter_token_value == EOS){
                printf("No jump required!\n");
                break;
            }   
            //jump to new table - initiate new table and save the post reduction state
            
            uint32_t return_state = new_state;
            uint32_t return_table = (jump_mask | (uint32_t)iterator->type);

            push(iterator->return_stack, &return_state, true);
            push(iterator->return_stack, &return_table, true);
            
            initiate_table(iterator, current_lookahead);
            
            return HOLD;

        }case(save_mask): {

            //save current state to the stack and jump to new state
            //we dereference it on the stack as its going to be changing
            new_state = new_state & 0x000fffff;
            printf("Saving state: %d\n", iterator->state);
            push(iterator->return_stack, &iterator->state, true);
            break;

        }case(sna_mask): {
            
            //save no jump mask - save current state, but dont advance token
            printf("Saving state: %d, but not advancing token\n", iterator->state);
            push(iterator->return_stack, &iterator->state, true);
            iterator->state = new_state & 0x000fffff;
            return HOLD;

        }case(open_parentheses): {
            //firstly push the intended state - after the brackets are reduced, we return to this state
            //then push C to the return stack, to indicate an open bracket
            //when closing a bracket, return states are popped, until C is found. this indicates open bracket
            new_state = open_expression_parentheses(iterator, new_state);
            break;
        }case(C): {

            return_to_previous_state(iterator);
            if(iterator->state == C){
                return_to_previous_state(iterator);
                return ADVANCE;
            }
            //still states in the bracket that need to be returned to
            return HOLD;
            
        }case(OB): {
            //open block

            if(iterator->return_stack->top > -1){
                return_to_previous_state(iterator);
                return HOLD;
            }

            return OPEN_BLOCK;
        }
        default:
            break;
    }

    //new state - keep pushing new ast nodes to stack
    iterator->state = new_state;
    //we dont need delimiters in the node stack
    if((*current_lookahead)->token_type != DELIMITER){    
        push_token_into_ast_node(iterator, current_lookahead, true);
    }
    return ADVANCE;
}

/** When an iterate has fully reduced and parsed a statement, it can be closed
 * 
 */
ASTNode* close_iterator(table_iterator* iterator){

    if(iterator->node_stack->top != 0){
        perror("Tried to close iterator (produce statement) without proper reduction\n");
    }

    iterator->initiated=0;
    iterator->specifiers=0x0000;
    iterator->table=NULL;
    return *(ASTNode**)pop(iterator->node_stack);
    
}

/** creates a new table iterator in memory and allocates memory for the stack 
 *  all allocating is done here for the AST
 */
table_iterator* initialize_table_iterator(statement_list* global_slist){
    table_iterator* new_iterator = safe_malloc(sizeof(table_iterator));
    new_iterator->working_list = global_slist;
    
    new_iterator->node_stack = create_stack(sizeof(ASTNode*));
    new_iterator->return_stack = create_stack(sizeof(uint32_t));
    
    new_iterator->expr_table =      *get_expr_table();
    new_iterator->reserved_table =  *get_reserved_table();

    new_iterator->specifiers = 0x0000;

    new_iterator->table = NULL;

    new_iterator->node_pool = init_data_pool(sizeof(ASTNode), 20);

    new_iterator->initiated = 0;
    return new_iterator;
}