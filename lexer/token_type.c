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
    dictionary->free_list = (char*)malloc(sizeof(char)*dictionary->maximum_amount);
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

    create_new_tokentype(dictionary, "if", (token_values){ .reserved_word_token_value =     IF},        RESERVED_WORD);
    create_new_tokentype(dictionary, "else", (token_values){ .reserved_word_token_value =   ELSE},      RESERVED_WORD);


    return dictionary;
}

void expand_dictionary(tokentype_dictionary* dictionary){
    int max = dictionary->maximum_amount;
    int new_maximum_amount = max + DICTIONARY_EXPAND_AMOUNT;

    dictionary->dictionary = safe_realloc(dictionary->dictionary, sizeof(tokentype_dictionary_entry*)*new_maximum_amount);
    dictionary->free_list = safe_realloc(dictionary->free_list, new_maximum_amount);

    safe_memset( dictionary->free_list + max, 0, DICTIONARY_EXPAND_AMOUNT);

    dictionary->maximum_amount = new_maximum_amount;
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

/** Used to acquire the TYPE of token associated with a lexeme, i.e., "+" is ADDITION token_value, with OPERATOR function.
 * 
 */
tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, char* lexeme){
    for(int i=0; i<dictionary->maximum_amount; ++i){
        //check if free_list is 1 and the actual entry is not null
        if(dictionary->free_list[i] == 1 && dictionary->dictionary[i] != NULL){
            if (!strcmp(dictionary->dictionary[i]->lexeme, lexeme)){
                return dictionary->dictionary[i];
            }
        }
    }
    return NULL;
}

/** Produces token - either searches dictionary and acquires pre-existing token details (Like operator type from its associated lexeme, or identifier)
 * 
 */
token* produce_token(token* prev, tokentype_dictionary* dictionary, lexeme* lexeme){
    token* new_token = (token*)safe_malloc(sizeof(token));
    new_token->next = NULL;
    if(prev != NULL){
        new_token->previous = (struct token*)prev;
        prev->next = (struct token*)new_token;
    }

    if(lexeme->type == STRING_LITERAL || lexeme->type == INT_VALUE){
        //int or string lexeme
        new_token->token_type = lexeme->type;
        new_token->token_value = (token_values)lexeme->value;
        return new_token;
    }else{
        tokentype_dictionary_entry* tokentype = tokentype_lookup(dictionary, lexeme->value);
        if(tokentype != NULL){
            new_token->token_value = tokentype->token_value;
            new_token->token_type = tokentype->token_types;

            return new_token;
        }else{
            create_new_tokentype(dictionary, lexeme->value, (token_values)lexeme->value, IDENTIFIER);
            new_token->token_value = (token_values)lexeme->value;
            new_token->token_type = IDENTIFIER;

            return new_token;
        }
    }
    return NULL;
}