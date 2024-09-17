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
#include "table_initiator.h"
#include "ast_utility/token_scanner.h"

void drop_table(table_iterator* iterator);

/** these tables allow us to find the index of the selected table
 *  for exmaple, in numbers table, operator enum 1 (subtraction) leads us to index 1
 *  int index (which only has one form), leads us to index 0 in the num table, 2 in the assignment table
 */
const uint32_t operator_index_lookup[][6] = {
    {1, 1,  2,  2},         //numbers table
    {N, N,  N,  N,  1,  N},  //assignment table
    {N,N}
};

const uint32_t delimiter_index_lookup[][5] = {
    {3,3,N,N,N},    //numbers table
    {5,N,N,N,N},    //assignment table
    {3,N,N,N,N},    //reserved table
};

const uint32_t int_index_lookup[3] = {
    0,  //numbers table
    2,  //assignemnt table
    1,  //reserved table
};

const uint32_t identifier_index_lookup[3] = {
    0,  //numbers table
    0,  //assignemnt table
    0,  //reserved table
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



            break;
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
    advance_token(current_lookahead);
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
        return ERROR;
    }

    printf("The next state is %d, pointed to by index %d, on current state %d\n",new_state, new_index, iterator->current->state);

    //finished parsing statement, passed to the iterator above
    if (new_state == A){
        if(iterator->progression_stack->top == -1){
            printf("Completed parsing\n");
            return COMPLETED;
        }else{
            printf("Returning to previous table\n");
            drop_table(iterator);
            return shift(iterator, current_lookahead);
        }
    
    }else if (new_state == N){
        perror("Unexpected symbol\n");
        return ERROR;
        //error

    }else if ( (reduction_mask & new_state) == reduction_mask ){
        //apply reduction rule and then push new token
        new_state = reduce(iterator->node_stack, new_state);
        //the reduction rule gives a new state to return to, then call again to push the lookahead
        if (new_state != N){
            iterator->current->state = new_state;
            return shift(iterator, current_lookahead);
        }
        perror("Invalid reduction\n");
        return ERROR;

    }else if ( (jump_mask & new_state) == jump_mask){

        //Firsly push current token into the stack
        push_token_into_ast_node(iterator, current_lookahead);
        int new_table = new_state & 0x000fffff;
        new_state = ( (new_state & 0x0ff00000) >> new_state_shift_count);
        iterator->current->state = new_state;
        //sets the current table to the state after parsing the table jumped to

        //firstly check the token after the current lookahead - if its a delimiter, we dont need to iterate the FSM - continue on current table to delimiter
        if((*current_lookahead)->next != NULL && (*current_lookahead)->next->token_type == DELIMITER){
            push_token_into_ast_node(iterator, current_lookahead);
            printf("No jump required!\n");
            return SHIFTED;
        }
        printf("Jumping to new table\n");
        
        //jump to new table - initiate new table
        initiate_table(iterator, current_lookahead, new_table);
        return JUMP;

    }else{
        //new state - keep pushing new ast nodes to stack
        iterator->current->state = new_state;
        push_token_into_ast_node(iterator, current_lookahead);
        return SHIFTED;
    }

    //no valid state found
    perror("No valid state found\n");
    return ERROR;
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

    return (ASTNode*)pop(iterator->node_stack);
    
}

/** initiates iterator with a new type of table
 * 
 */
void initiate_table(table_iterator* iterator, token** initiating_token, table_type type){
    printf("Initiating\n");
    if(iterator->current != NULL){
        //push current state onto the stack
        push(iterator->progression_stack, iterator->current);
    }

    //create a brand new table progression
    iterator->current = (table_progression*)acquire_from_pool(iterator->progression_pool);
    iterator->current->state = 0;
    iterator->current->table = NULL;
    iterator->current->type = NONE;

    initiate_statement(initiating_token, iterator);

    iterator->initiated = 1;

    push_token_into_ast_node(iterator, initiating_token);
}

void drop_table(table_iterator* iterator){
    return_to_pool(iterator->progression_pool, iterator->current);
    iterator->current = (table_progression*)pop(iterator->progression_stack);
}

/** creates a new table iterator in memory and allocates memory for the stack 
 *  all allocating is done here for the AST
 */
table_iterator* initialize_table_iterator(void){
    table_iterator* new_iterator = safe_malloc(sizeof(table_iterator));
    new_iterator->node_stack = create_stack(sizeof(ASTNode*));
    new_iterator->progression_stack = create_stack(sizeof(table_progression));
    new_iterator->progression_pool = init_data_pool(sizeof(table_progression), 10);

    new_iterator->current = NULL;

    new_iterator->node_pool = init_data_pool(sizeof(ASTNode), 20);

    new_iterator->initiated = 0;
    return new_iterator;
}