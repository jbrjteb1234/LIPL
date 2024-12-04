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
#include "ast_utility/routines.h"
#include "table_management/token_converter.h"

/** interprets next token in the stream - 
 *  iterates state
 */
shift_results shift(table_iterator* iterator, token** current_lookahead){

    //convert token to astnode, push to the stack, and handle the new state
    uint32_t new_index = convert_token(iterator,  current_lookahead);
    uint32_t new_state;
    
    switch(new_index){

        case(N):
            return ERROR;

        default:
            new_state = iterator->table[iterator->state][new_index];
            break;
    }

    //TODO: HANDLE INDEX RESULT

    printf("The next state is %d, pointed to by index %d, on current state %d\n",new_state, new_index, iterator->state);
    uint32_t new_state_type = new_state & general_mask;
    //finished parsing statement, passed to the iterator above
    switch(new_state_type){
        case(A): {
            //firstly, check if theres a saved state to return to
            if(iterator->return_stack->top > -1){
                return_to_previous_state(iterator);
                return HOLD;
            //if no saved states and no other table to return to, then the statement is completed
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
            
            uint32_t open_bracket_state_marker = C;
            uint32_t state_after_close = new_state & 0x000fffff;
            uint32_t bracket_state = (new_state & 0x0ff00000) >> open_parentheses_state_shift_count;
            new_state = bracket_state;
            printf("Opening bracket via state: %d\n", bracket_state);
            push(iterator->return_stack, &state_after_close, true);
            push(iterator->return_stack, &open_bracket_state_marker, true);
            
            return ADVANCE;

        }case(C): {

            return_to_previous_state(iterator);
            if(iterator->state == C){
                return_to_previous_state(iterator);
                return ADVANCE;
            }
            //still states in the bracket that need to be returned to
            return HOLD;
        
        }case(OB):
            return OPEN_BLOCK;

        case(CB):
            return CLOSE_BLOCK;
        
        default:
            break;
    }

    //new state - keep pushing new ast nodes to stack
    iterator->state = new_state;
    return ADVANCE;
}

/** When an iterate has fully reduced and parsed a statement, it can be closed
 * 
 */
ASTNode* close_iterator(table_iterator* iterator){

    if(iterator->node_stack->top != 0){
        perror("Tried to close iterator (produce statement) without proper reduction\n");
    }

    iterator->state=0;
    iterator->specifiers=0x0000;
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

    new_iterator->specifiers = 0x0000;

    new_iterator->table = *get_state_table();
    new_iterator->state = 0;

    new_iterator->node_pool = init_data_pool(sizeof(ASTNode), 20);

    return new_iterator;
}