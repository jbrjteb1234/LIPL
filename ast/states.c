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

#define NUM_TABLE_HEIGHT 7
#define DECL_TABLE_HEIGHT 9

typedef uint32_t numbers_table_type[NUM_TABLE_HEIGHT][WIDTH];
typedef uint32_t decl_table_type[DECL_TABLE_HEIGHT][WIDTH];


//      R(0) = reduce binary operator (like 1+1 or 2*1)
//      0 num/id    1 +-    2 */    3 eos/eb

/** STRUCTURE OF A TABLE ENTRY
 *      half-byte 1 = call mask (type of call it is (reduction, normal state, etc))
 *      half-byte 2/3 = return state (where to return to after reduction)
 *      half-byte 4-8 = data    
 */

numbers_table_type numbers_table = {
        {N,         1,      2,      A},             //STATE 0: num
        {3,         N,      N,      N},             //STATE 1: num +-
        {4,         N,      N,      N},             //STATE 2: num */

        {N,         R(0,0), 5,      R(0,0)},        //STATE 3: num +- num
        {N,         R(0,0), R(0,0), R(0,0)},        //STATE 4: num */ num

        {6,         N,      N,      N},             //STATE 5: num +- num *
        {N,         R(0,3), R(0,3), R(0,3)}         //STATE 6: num +- num * num
};

//      0 ID    1 =     2 num   3 str   4 op    5 del       
decl_table_type decl_table = {
        {N,     2,      N,      N,      N,      N},         //STATE 0: ID
        {N,     N,      J(0),   N,      N,      N},         //STATE 1: ID =
        {N,     N,      N,      N,      N,      R(1,8)},    //STATE 2: ID = num
        {N,     N,      N,      N,      N,      R(1,8)},    //STATE 3: ID = str
        {N,     N,      N,      N,      N,      R(1,8)},    //STATE 4: ID = ID
        
        {N,     N,      N,      N,      N,      N},         //STATE 5: spare
        {N,     N,      N,      N,      N,      N},         //STATE 6: spare
        {N,     N,      N,      N,      N,      N},         //STATE 7: spare

        {N,     N,      N,      N,      N,      N},         //STATE 8: DECLARATION
};

/** these tables allow us to find the index of the selected table
 *  for exmaple, in numbers table, operator enum 1 (subtraction) leads us to index 1
 *  int index (which only has one form), leads us to index 0 in the num table, 2 in the decl table
 */
const uint32_t operator_index_lookup[][6] = {
    {1, 1,  2,  2},         //numbers table
    {N, N,  N,  N,  N,  1}  //decl table
};

const uint32_t delimiter_index_lookup[][5] = {
    {3,3,N,N,N},    //numbers table
    {5,N,N,N,N}     //decl table
};

const uint32_t int_index_lookup[2] = {
    0,  //numbers table
    2   //decl table
};

const uint32_t identifier_index_lookup[2] = {
    0,  //numbers table
    0,  //decl table
};

/** Returns the index of the table based on the token, acquired from the tables
 * 
 */
uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead){
    switch(current_lookahead->token_type){
        case(RESERVED_WORD):

            

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
}

/** converts token directly from tokenstream into an AST node
 *  adds AST node to the stack for reduction
 */
void push_token_into_ast_node(table_iterator* iterator, token* current_lookahead){

    if (current_lookahead == NULL){
        return;
    }

    ASTNode* new_ast_node = safe_malloc(sizeof(ASTNode));
    new_ast_node->token = current_lookahead;

    //if it is a leaf node  then we can transfer data from token to node immedietly
    if(current_lookahead->leaf == 1){
        new_ast_node->leaf_node=1;
        if(current_lookahead->token_type == IDENTIFIER){
            new_ast_node->data.value_node.identifier = current_lookahead->token_value.identifier_token_value;
        }else{
            new_ast_node->data.value_node.value = current_lookahead->token_value.variable_value;
        }
    }else{
        new_ast_node->leaf_node = 0;
    }

    push(iterator->node_stack, new_ast_node);
}

/** interprets next token in the stream - 
 *  iterates state
 */
bool shift(table_iterator* iterator, token* current_lookahead){
    
    if (iterator->current->table == NULL){
        //error
        perror("Table not initiated\n");
        return false;
    }

    //finds the index in the table that the token points to
    uint32_t new_index = convert_token_to_index(iterator, current_lookahead);
    uint32_t new_state;

    //if new_index is N, then an unrecognised symbol has appeared
    if(new_index != N){
        //acquire the new state (could be reduction, error, completion or another state)
        new_state = iterator->current->table[iterator->current->state][new_index];
    }else{
        //error - unrecognised symbol
        return false;
    }

    printf("The next state is %d, pointed to by index %d, on current state %d\n",new_state, new_index, iterator->current->state);

    //finished parsing statement, passed to the iterator above
    if (new_state == A){
        printf("Complete\n");
        return true;
        //completed
    
    }else if (new_state == N){
        perror("Unexpected symbol\n");
        //error

    }else if ( (reduction_mask & new_state) == reduction_mask ){
        //apply reduction rule and then push new token
        new_state = reduce(iterator->node_stack, new_state);
        //the reduction rule gives a new state to return to, then call again to push the lookahead
        if (new_state != N){
            iterator->current->state = new_state;
            shift(iterator, current_lookahead);
        }

    }else if ( (jump_mask & new_state) == jump_mask){
        printf("Jumping to new table\n");
        //Jump! its time to create a new table progession
        //firstly check the token after the current lookahead - if its a delimiter, we dont need to iterate the FSM
        if(current_lookahead->next->token_type == DELIMITER){
            push_token_into_ast_node(iterator, current_lookahead);
            return false;
        }

    }else{
        //new state - keep pushing new ast nodes to stack
        iterator->current->state = new_state;
        push_token_into_ast_node(iterator, current_lookahead);
        return false;
    }

}

/** When an iterate has fully reduced and parsed a stream of tokens, it can be closed
 * 
 */
ASTNode* close_iterator(table_iterator* iterator, statement_list* current_working_list){

    reset_pool(iterator->progression_pool);
    return (ASTNode*)pop(iterator->node_stack);
    
}

/** initiates iterator with a new type of table
 * 
 */
void initiate_table(table_iterator* iterator, token* initiating_token){

    iterator->current = (table_progression*)acquire_from_pool(iterator->progression_pool);
    iterator->current->state = 0;
    iterator->current->table = NULL;
    iterator->current->type = N;

    //initial state for all tables
    iterator->current->state = 0;
    push_token_into_ast_node(iterator, initiating_token);
    printf("Token type: %d\n", initiating_token->token_type);
    switch(initiating_token->token_type){
        case(RESERVED_WORD):
            break;
        case(OPERATOR):
            break;
        case(DELIMITER):
            break;
        case(STRING_LITERAL):
            break;
        case(INT_VALUE):
            printf("Initialising numbers table\n");
            iterator->current->type = NUMBERS_TABLE;
            iterator->current->table = numbers_table;
            break;
        case(IDENTIFIER):
            break;
    }
    iterator->initiated = 1;
}

/** creates a new table in memory and allocates memory for the stack 
 * 
 */
table_iterator* initialize_table_iterator(){
    table_iterator* new_iterator = safe_malloc(sizeof(table_iterator));
    new_iterator->node_stack = create_stack(sizeof(ASTNode*));
    new_iterator->progression_stack = create_stack(sizeof(table_progression));
    new_iterator->progression_pool = init_data_pool(sizeof(table_progression));
    new_iterator->initiated = 0;
    return new_iterator;
}