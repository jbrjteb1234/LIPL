#include "token_to_index.h"
#include <stdlib.h>
#include <stdio.h>

const uint32_t operator_index_lookup[][12] = {
    {2,2,3,3,6,10,10,10,10,10,10,5},         //var table
    {N,N,N,N,3,N,N,N,N,N,N,N}
};

const uint32_t delimiter_index_lookup[][6] = {
    {4,8,9,11,N,7},    //var table
    {2,3,4,5,N,N},    //reserved table
};

const uint32_t int_index_lookup[3] = {
    0,  //var table
    1,  //reserved table
};

const uint32_t identifier_index_lookup[3] = {
    1,  //var table
    0,  //reserved table
};

const uint32_t string_index_lookup[3] = {
    0,  //var table
    0,  //reserved table
};

/** Returns the index of the table based on the token, acquired from the tables
 * 
 */
uint32_t convert_token_to_index(table_iterator* iterator, token* current_lookahead){

    if (iterator->initiated == 0 || iterator->table == NULL){
        if( current_lookahead->token_type == DELIMITER && current_lookahead->token_value.delimiter_token_value == CLOSE_CBRACKET){
            return CB;
        }
        return NI;
    }
    
    switch(current_lookahead->token_type){
        case(RESERVED_WORD):

            return N;

        case(OPERATOR):

            return operator_index_lookup[iterator->type][current_lookahead->token_value.operator_token_value];

            break;
        case(DELIMITER):

            if( current_lookahead->token_type == DELIMITER && current_lookahead->token_value.delimiter_token_value == CLOSE_CBRACKET){
                return CB;
            }

            return delimiter_index_lookup[iterator->type][current_lookahead->token_value.delimiter_token_value];

            break;

        case(STRING_LITERAL):
        case(INT_VALUE):

            return int_index_lookup[iterator->type];
        
            break;
        case(IDENTIFIER):

            return identifier_index_lookup[iterator->type];

            break;
        default:
            break;
    }
    //unrecognised symbol (erronoeus token-type)
    perror("Unrecognised token-type\n");
    return N;
}
