#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token_type.h"
#include "token.h"
#include "../utility/safe_memory.h"

/** functionality for managing the relation between lexemes and different types of tokens
 *  allows for declaring identifier tokens
 */

tokentype_dictionary* initialize_tokentype_dictionary(){
    tokentype_dictionary* dictionary = safe_malloc(sizeof(tokentype_dictionary));
    dictionary->maximum_amount = INITIAL_DICTIONARY_SIZE;
    dictionary->dictionary = (tokentype_dictionary_entry**)safe_malloc(sizeof(tokentype_dictionary_entry*)*dictionary->maximum_amount);

    create_new_tokentype(dictionary, "=", (token_values){ .operator_token_value = ASSIGNMENT}, OPERATOR);

}

void create_new_tokentype(tokentype_dictionary* dictionary, char* lexeme, token_values value, token_functions function){
    tokentype_dictionary_entry* new_entry = (tokentype_dictionary_entry*)safe_malloc(sizeof(tokentype_dictionary_entry));
    new_entry->token_value = value;
    new_entry->token_functions = function;

    if(dictionary->index < dictionary->maximum_amount){
        dictionary->dictionary[dictionary->index] = new_entry;
        ++dictionary->index;
    }
}

token* produce_token(token* prev, char* lexeme){
    token* new_token = (token*)safe_malloc(sizeof(token));
    new_token->previous = (struct token*)prev; 
}