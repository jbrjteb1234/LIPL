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
#include "../lexer/token_type.h"

/** interprets next token in the stream - 
 *  iterates state
 */
shift_results shift(table_iterator* iterator, token** current_lookahead){

    //convert token to astnode, push to the stack, and handle the new state
    uint32_t new_index = convert_token(iterator,  current_lookahead);
    uint32_t new_state = N;
    
    new_state = iterator->table[iterator->state][new_index];

    printf("The next state is %u, pointed to by index %u, on current state %u\n",new_state, new_index, iterator->state);
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

            if(return_state != NULL && *return_state != O && iterator->return_stack->top > -1){
                //check if there is states to return to. if there is a state to return to, apply virtual shift to handle that state with the reduced node
                return_to_previous_state(iterator);
                if(iterator->state == save_with_vs_mask){
                    return_to_previous_state(iterator);
                    apply_virtual_shift(iterator);
                }
                return HOLD;
            }
            //REMOVE RETURN STATE FOR REDUCTION CALLS AND MAKE IT DEFAULT TO EXPRESSION STATE
            //the reduction rule gives a new state to return to, then call again to push the lookahead
            if (new_state != N){
                iterator->state = new_state;
                return HOLD;
            }
            perror("Invalid reduction\n");
            return ERROR;

        }case(save_mask): {

            //save a reducable state
            printf("Saving state: %d\n", iterator->state);
            push(iterator->return_stack, &iterator->state, true);
            iterator->state = new_state & 0x000fffff;
            return ADVANCE;
        
        }case(save_with_vs_mask): {

            //save a non-reducable state, will require a virtual shift (push expr) to move to intended state
            //save a reducable state
            printf("Saving state: %d and marking a virtual shift\n", iterator->state);
            push(iterator->return_stack, &iterator->state, true);
            uint32_t vs_state = save_with_vs_mask;
            push(iterator->return_stack, &vs_state, true);
            iterator->state = new_state & 0x000fffff;
            return ADVANCE;

        }case(O): {
            
            //push state before the bracket, then push an open bracket symbol
            uint32_t o_state = O;
            printf("Opening bracket and saving state: %d\n", iterator->state);
            push(iterator->return_stack, &iterator->state, true);
            iterator->state = INIT_STATE;
            push(iterator->return_stack, &o_state, true);
            
            return ADVANCE;

        }case(C): {

            //pop any unreduced states. if its O, pop again to return to pre bracket state 

            return_to_previous_state(iterator);
            
            if(iterator->state == O){
                return_to_previous_state(iterator);
                apply_virtual_shift(iterator);
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

    new_iterator->table = *get_state_table();
    new_iterator->state = 0;

    new_iterator->new_node_buffer = NULL;
    new_iterator->new_node_buffer_set_flag = false;

    new_iterator->node_pool = init_data_pool(sizeof(ASTNode), 20);

    return new_iterator;
}