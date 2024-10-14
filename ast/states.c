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

void drop_table(table_iterator* iterator);

/** these tables allow us to find the index of the selected table
 *  for exmaple, in numbers table, operator enum 1 (subtraction) leads us to index 1
 *  int index (which only has one form), leads us to index 0 in the num table, 2 in the assignment table
 */
const uint32_t operator_index_lookup[][12] = {
    {1, 1,  2,  2,5,N,N,N,N,N,N,4},         //var table
    {N, N,  N,  N,  1,  N},  //assignment table
    {N,N,N,N,4}
};

const uint32_t delimiter_index_lookup[][6] = {
    {3,7,8,N,N,6},    //var table
    {5,N,N,N,N,N},    //assignment table
    {3,N,N,N,N,N},    //reserved table
};

const uint32_t int_index_lookup[3] = {
    0,  //var table
    2,  //assignemnt table
    1,  //reserved table
};

const uint32_t identifier_index_lookup[3] = {
    0,  //var table
    0,  //assignemnt table
    0,  //reserved table
};

const uint32_t string_index_lookup[3] = {
    N,  //var table
    N,  //assignemnt table
    N,  //reserved table
};

/** Returns the index of the table based on the token, acquired from the tables
 * 
 */
uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead){
    switch(current_lookahead->token_type){
        case(RESERVED_WORD):

            //TODO: PARSE RESERVED WORDS

            //since reserved words differ in how they are parsed, we need seperate tables for each reserved word(s)

            break;
        case(OPERATOR):

            return operator_index_lookup[iterator->current->type][current_lookahead->token_value.operator_token_value];

            break;
        case(DELIMITER):

            return delimiter_index_lookup[iterator->current->type][current_lookahead->token_value.delimiter_token_value];

            break;

        case(STRING_LITERAL):
        case(INT_VALUE):

            return int_index_lookup[iterator->current->type];
        
            break;
        case(IDENTIFIER):

            return identifier_index_lookup[iterator->current->type];

            break;
        default:
            break;
    }
    //unrecognised symbol (erronoeus token-type)
    perror("Unrecognised token-type\n");
    return N;
}

/** interprets next token in the stream - 
 *  iterates state
 */
shift_results shift(table_iterator* iterator, token** current_lookahead){
    if (iterator->current->table == NULL){
        //error
        perror("Table not initiated\n");
        return ERROR;
    }

    //finds the index in the table that the token points to
    uint32_t new_index = convert_token_to_index(iterator, *current_lookahead);
    uint32_t new_state;
    //if new_index is N, then an unrecognised symbol has appeared
    if(new_index != N){
        //acquire the new state (could be reduction, error, completion or another state)
        new_state = iterator->current->table[iterator->current->state][new_index];
    }else{
        //error - unrecognised symbol
        perror("Unrecognised symbol\n");
        return ERROR;
    }

    printf("The next state is %d, pointed to by index %d, on current state %d\n",new_state, new_index, iterator->current->state);
    uint32_t new_state_type = new_state & general_mask;

    //finished parsing statement, passed to the iterator above
    switch(new_state_type){
        case(A): {
            //firstly, check if theres a saved state to return to
            if(iterator->current->return_stack->top > -1){
                new_state = *(uint32_t*)pop(iterator->current->return_stack);
                iterator->current->state = new_state;
                printf("Returning to state %d\n", new_state);
                return shift(iterator, current_lookahead);
            //also check if there is are any parentheses to close, if not, error
            }else if(iterator->parentheses_stack->top > -1){
                perror("Unmatched parentheses\n");
                return ERROR;
            //then, check if theres any other table progressions to return to
            }else if(iterator->progression_stack->top > -1){
                printf("Returning to previous table\n");
                drop_table(iterator);
                return shift(iterator, current_lookahead);
            //if no saved states and no other tables to return to, then the statement is completed
            }else{
                printf("Completed parsing\n");
                //advance one to skip the EOS token
                advance_token(current_lookahead);
                return COMPLETED;
            }
        
        }case(N): {
            perror("Unexpected symbol\n");
            return ERROR;
            //error

        }case(reduction_mask): {
            //apply reduction rule and then push new token
            new_state = reduce(iterator, new_state);
            //the reduction rule gives a new state to return to, then call again to push the lookahead
            if (new_state != N){
                iterator->current->state = new_state;
                return shift(iterator, current_lookahead);
            }
            perror("Invalid reduction\n");
            return ERROR;

        }case(jump_mask): {
            
            //Firsly push current token into the stack
            int new_table = new_state & 0x000fffff;
            new_state = ( (new_state & 0x0ff00000) >> new_state_shift_count);
            iterator->current->state = new_state;
            //sets the current table to the state after parsing the table jumped to

            //firstly check the token after the current lookahead - if its a delimiter, we dont need to iterate the FSM - continue on current table to delimiter
            if((*current_lookahead)->next != NULL && (*current_lookahead)->next->token_type == DELIMITER && (*current_lookahead)->next->token_value.delimiter_token_value == EOS){
                printf("No jump required!\n");
                break;
            }
            printf("Jumping to new table\n");
            
            //jump to new table - initiate new table
            initiate_table(iterator, current_lookahead, new_table);
            return JUMP;
        }case(save_mask): {

            //save current state to the stack and jump to new state
            //we dereference it on the stack as its going to be changing
            new_state = new_state & 0x000fffff;
            printf("Saving state: %d\n", iterator->current->state);
            push(iterator->current->return_stack, &iterator->current->state, true);
            break;
        }case(open_parentheses): {
            //push C to the return stack, to indicate an open bracket
            //when closing a bracket, return states are popped, until C is found. this indicates open bracket
            new_state = open_expression_parentheses(iterator, new_state);
            break;
        }case(C): {
            new_state = *(uint32_t*)pop(iterator->current->return_stack);
            if(new_state == C){
                //parentheses closed, we can now return to the intended state
                printf("Closing bracket\n");
                new_state = *(uint32_t*)pop(iterator->current->return_stack);
                break;
            }else{
                //Still saved state in the parentheses stack, we can continue parsing
                iterator->current->state = new_state;
                return shift(iterator, current_lookahead);
            }
        }
        default:
            break;
    }

    //new state - keep pushing new ast nodes to stack
    iterator->current->state = new_state;
    //we dont need delimiters in the node stack
    if((*current_lookahead)->token_type != DELIMITER){    
        push_token_into_ast_node(iterator, current_lookahead, true);
    }
    return SHIFTED;
}

/** When an iterate has fully reduced and parsed a statement, it can be closed
 * 
 */
ASTNode* close_iterator(table_iterator* iterator){

    reset_pool(iterator->progression_pool);

    if(iterator->node_stack->top != 0){
        perror("Tried to close iterator (produce statement) without proper reduction\n");
    }

    iterator->initiated=0;
    iterator->current=NULL;

    return *(ASTNode**)pop(iterator->node_stack);
    
}

/** initiates iterator with a new type of table
 * 
 */
void initiate_table(table_iterator* iterator, token** initiating_token, table_type type){
    printf("Initiating\n");
    if(iterator->current != NULL){
        //push current state onto the stack
        printf("Pushin new table onto stack\n");
        push(iterator->progression_stack, iterator->current, false);
    }

    //create a brand new table progression
    iterator->current = (table_progression*)acquire_from_pool(iterator->progression_pool);
    iterator->current->state = 0;
    iterator->current->table = NULL;
    iterator->current->type = NONE_TABLE;
    iterator->current->return_stack = create_stack(sizeof(uint32_t));

    initiate_statement(initiating_token, iterator);

    iterator->initiated = 1;
}

void drop_table(table_iterator* iterator){
    return_to_pool(iterator->progression_pool, iterator->current);
    iterator->current = *(table_progression**)pop(iterator->progression_stack);
}

/** creates a new table iterator in memory and allocates memory for the stack 
 *  all allocating is done here for the AST
 */
table_iterator* initialize_table_iterator(void){
    table_iterator* new_iterator = safe_malloc(sizeof(table_iterator));
    new_iterator->node_stack = create_stack(sizeof(ASTNode*));
    new_iterator->parentheses_stack = create_stack(sizeof(uint32_t));
    new_iterator->progression_stack = create_stack(sizeof(table_progression*));
    new_iterator->progression_pool = init_data_pool(sizeof(table_progression), 10);

    new_iterator->current = NULL;

    new_iterator->node_pool = init_data_pool(sizeof(ASTNode), 20);

    new_iterator->initiated = 0;
    return new_iterator;
}