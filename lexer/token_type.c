#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token_type.h"
#include "token.h"
#include "../utility/safe_memory.h"
#include "../grammar/grammar.h"

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

    #define X(LEXEME, VALUE, FUNCTION, PRECEDENCE) create_new_tokentype(dictionary, LEXEME, (token_values){ .operator_token_value = VALUE }, FUNCTION, (unsigned char)PRECEDENCE);
    DEFINE_TOKENS
    #undef X


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
void create_new_tokentype(tokentype_dictionary* dictionary, char* lexeme, token_values value, token_types function, unsigned char precedence){
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
            new_entry->precedence = precedence;
            strcpy(new_entry->lexeme, lexeme);

            dictionary->dictionary[i] = new_entry;

            return;
        }
    }
    //dictionary full - expand and attempt creation again
    expand_dictionary(dictionary);
    create_new_tokentype(dictionary, lexeme, value, function, precedence);
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
        new_token->precedence = LITERAL_PRECEDENCE;
        return new_token;
    }else{
        tokentype_dictionary_entry* tokentype = tokentype_lookup(dictionary, lexeme->value);
        if(tokentype != NULL){
            //tokentype already exists, use existing tokentype data
            new_token->token_value = tokentype->token_value;
            new_token->token_type = tokentype->token_types;
            new_token->precedence = tokentype->precedence;
            return new_token;
        }else{
            //tokentype does not exist - create a new tokentype
            create_new_tokentype(dictionary, lexeme->value, (token_values)lexeme->value, IDENTIFIER, IDENTIFIER_PRECEDENCE);
            new_token->token_value = (token_values)lexeme->value;
            new_token->token_type = IDENTIFIER;

            return new_token;
        }
    }
    return NULL;
}