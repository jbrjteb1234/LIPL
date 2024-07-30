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

    //create dictionary (array of tokentype_dictionary_entry pointers)
    dictionary->dictionary = (tokentype_dictionary_entry**)safe_malloc(sizeof(tokentype_dictionary_entry*)*dictionary->maximum_amount);

    //create free list, set all indexes to 0
    dictionary->free_list = (char*)safe_malloc(sizeof(char)*dictionary->maximum_amount);
    safe_memset(dictionary->free_list, 0, dictionary->maximum_amount);

    create_new_tokentype(dictionary, "=", (token_values){ .operator_token_value =   ASSIGNMENT},        OPERATOR);
    create_new_tokentype(dictionary, "+", (token_values){ .operator_token_value =   ADDITION},          OPERATOR);
    create_new_tokentype(dictionary, "-", (token_values){ .operator_token_value =   SUBTRACTION},       OPERATOR);
    create_new_tokentype(dictionary, "*", (token_values){ .operator_token_value =   ASSIGNMENT},        OPERATOR);
    create_new_tokentype(dictionary, "/", (token_values){ .operator_token_value =   DIVISION},          OPERATOR);
    create_new_tokentype(dictionary, "<", (token_values){ .operator_token_value =   LESS_THAN},         OPERATOR);
    create_new_tokentype(dictionary, ">", (token_values){ .operator_token_value =   GREATER_THAN},      OPERATOR);
    create_new_tokentype(dictionary, "=<", (token_values){ .operator_token_value =  LESS_OR_EQUAL},     OPERATOR);
    create_new_tokentype(dictionary, ">=", (token_values){ .operator_token_value =  GREATER_OR_EQUAL},  OPERATOR);
    create_new_tokentype(dictionary, "==", (token_values){ .operator_token_value =  EQUIVALENT},        OPERATOR);
    create_new_tokentype(dictionary, "!=", (token_values){ .operator_token_value =  NOT_EQUIVALENT},    OPERATOR);


}

void expand_dictionary(tokentype_dictionary* dictionary){
    //TODO - create expand_dictionary function
}

/** initialises new type of token
 * 
 */
void create_new_tokentype(tokentype_dictionary* dictionary, char* lexeme, token_values value, token_functions function){
    if(lexeme==NULL){
        return;
    }
    for(int i=0; i<dictionary->maximum_amount; ++i){
        if(dictionary->free_list[i] == 0){

            dictionary->free_list[i] = 1;
            tokentype_dictionary_entry* new_entry = (tokentype_dictionary_entry*)safe_malloc(sizeof(tokentype_dictionary_entry));
            new_entry->token_value = value;
            new_entry->token_functions = function;
            new_entry->lexeme = (char*)safe_malloc(strlen(lexeme) + 1);
            strcpy(new_entry->lexeme, lexeme);

            dictionary->dictionary[i] = new_entry;

            return;
        }
    }
    expand_dictionary(dictionary);
    //create_new_tokentype(dictionary, lexeme, value, function);
}

tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, char* lexeme){
    for(int i=0; i<dictionary->maximum_amount; ++i){
        if(dictionary->free_list[i] == 1 && !strcmp(dictionary->dictionary[i]->lexeme, lexeme)){
            return dictionary->dictionary[i];
        }
    }
}

token* produce_token(token* prev, tokentype_dictionary* dictionary, char* lexeme){
    token* new_token = (token*)safe_malloc(sizeof(token));
    new_token->previous = (struct token*)prev; 

    if(1==2){
        //TODO - detecting if a lexeme is a literal
    }else{
        tokentype_dictionary_entry* tokentype = tokentype_lookup(dictionary, lexeme);
        //TODO - create token based off tokentype_dictionary_entry
    }
}