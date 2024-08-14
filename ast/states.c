#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "states.h"
#include "../utility/safe_memory.h"
#include "../lexer/token.h"
#include "../grammar/grammar.h"
#include "../utility/stack.h"

//      R(0) = reduce binary operator (like 1+1 or 2*1)
//      0 num   1 +-    3 */    4 del      
const int numbers_table[][4] = {
        {N,     1,      2,      A},         //STATE 0: num
        {3,     N,      N,      N},         //STATE 1: num +-
        {4,     N,      N,      N},         //STATE 2: num */

        {N,     R(0),   5,      R(0)},      //STATE 3: num +- num
        {N,     R(0),   R(0),   R(0)},      //STATE 4: num */ num

        {6,     N,      N,      N},         //STATE 5: num +- num *
        {N,     R(0),   R(0),   4}          //STATE 6: num +- num * num
};

//      0 ID    1 =     2 num   3 str   4 op    5 del       
const int decl_table[][6] = {
        {N,     2,      N,      N,      N,      N},         //STATE 0: ID
        {N,     N,      N,      N,      N,      N},         //STATE 1: ID =
        {N,     N,      N,      N,      N,      N},         //STATE 2: ID = num
        {N,     N,      N,      N,      N,      N},         //STATE 3: ID = str
        {N,     N,      N,      N,      N,      N},         //STATE 4: ID = ID
        
        {N,     N,      N,      N,      N,      N},         //STATE 5: ID = num (op)
        {N,     N,      N,      N,      N,      N},         //STATE 6: ID = str (op)
        {N,     N,      N,      N,      N,      N}          //STATE 7: ID = ID (op)
};

void reduce(stack* node_stack, int reduction){

}

int convert_token_to_index(table_iterator* iterator, token* current_token){
    switch (iterator->type){
        case NUMBERS_TABLE:


            break;
    
        case DECL_TABLE:

            break;

        default:
            break;
    }
}

void shift(table_iterator* iterator, stack* token_stack, token* current_token){
    
}

void clear_table_iterator(table_iterator* iterator){

}

void initiate_table(table_iterator* iterator, token* initiating_token){
    //initial state for all tables
    iterator->state = 0;
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

            iterator->type = NUMBERS_TABLE;
            iterator->table = numbers_table;

            break;

        case(IDENTIFIER):

            break;

        default:
            break;
    }
}

table_iterator* initialize_table_iterator(){
    table_iterator* new_iterator = safe_malloc(sizeof(new_iterator));
    new_iterator->state = 0;
    new_iterator->table = NULL;
    new_iterator->type = NULL;
}