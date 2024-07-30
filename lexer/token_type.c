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

    //printf("%s", dictionary->free_list);

}

void expand_dictionary(tokentype_dictionary* dictionary){
    int max = dictionary->maximum_amount;
    int new_maximum_amount = max + DICTIONARY_EXPAND_AMOUNT;

    tokentype_dictionary_entry** new_dictionary = (tokentype_dictionary_entry**)safe_malloc(sizeof(tokentype_dictionary_entry*)*new_maximum_amount);
    char* new_free_list = (char*)safe_malloc(sizeof(char)*new_maximum_amount);

    safe_memcpy(new_dictionary, dictionary->dictionary, sizeof(tokentype_dictionary_entry*) * max);
    safe_memcpy(new_free_list, dictionary->free_list, sizeof(char) * max);
    safe_memset( (char*)new_free_list + sizeof(char)*max, 0, DICTIONARY_EXPAND_AMOUNT);

    safe_free( (void**)dictionary->dictionary );
    safe_free( (void**)dictionary->free_list );

    dictionary->dictionary = new_dictionary;
    dictionary->free_list = new_free_list;
}

/** initialises new type of token
 * 
 */
void create_new_tokentype(tokentype_dictionary* dictionary, char* lexeme, token_values value, token_types function){
    if(lexeme==NULL){
        return;
    }
    for(int i=0; i<dictionary->maximum_amount; ++i){
        if(dictionary->free_list[i] == 0){

            dictionary->free_list[i] = 1;
            tokentype_dictionary_entry* new_entry = (tokentype_dictionary_entry*)safe_malloc(sizeof(tokentype_dictionary_entry));
            new_entry->token_value = value;
            new_entry->token_types = function;
            new_entry->lexeme = (char*)safe_malloc(strlen(lexeme) + 1);
            strcpy(new_entry->lexeme, lexeme);

            dictionary->dictionary[i] = new_entry;

            return;
        }
    }
    expand_dictionary(dictionary);
    create_new_tokentype(dictionary, lexeme, value, function);
}

tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, char* lexeme){
    for(int i=0; i<dictionary->maximum_amount; ++i){
        if(dictionary->free_list[i] == 1){
            //TODO - fix seg fault here
            //if (!strcmp(dictionary->dictionary[i]->lexeme, lexeme)){
            //    return dictionary->dictionary[i];
            //}
        }
    }
    return NULL;
}

token* produce_token(token* prev, tokentype_dictionary* dictionary, char* lexeme){
    token* new_token = (token*)safe_malloc(sizeof(token));
    new_token->previous = (struct token*)prev; 

    if(1==2){
        //TODO - detecting if a lexeme is a literal
    }else{
        tokentype_dictionary_entry* tokentype = tokentype_lookup(dictionary, lexeme);
        //TODO - create token based off tokentype_dictionary_entry
        if(tokentype != NULL){
            new_token->token_value = tokentype->token_value;
            new_token->token_type = tokentype->token_types;

            return new_token;
        }else{
            printf("Lexeme: %s unknown\n", lexeme);
            safe_free( (void**)&new_token );
            return NULL;
        }
    }
}