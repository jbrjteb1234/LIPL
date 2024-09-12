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

tokentype_dictionary* initialize_tokentype_dictionary(void){
    tokentype_dictionary* dictionary = safe_malloc((size_t) sizeof(tokentype_dictionary));
    dictionary->maximum_amount = INITIAL_DICTIONARY_SIZE;

    //create dictionary (array of tokentype_dictionary_entry pointers)
    dictionary->dictionary = (tokentype_dictionary_entry**)safe_malloc( (size_t) (sizeof(tokentype_dictionary_entry*)*dictionary->maximum_amount) );

    //create free list, set all indexes to 0
    dictionary->free_list = (char*)safe_malloc( (size_t) (sizeof(char)*dictionary->maximum_amount) );
    safe_memset(dictionary->free_list, 0, (size_t)dictionary->maximum_amount);

    #define X(LEXEME, VALUE, VALUE_TYPE, FUNCTION) \
    create_new_tokentype(dictionary, LEXEME, (token_values){ .VALUE_TYPE = VALUE }, FUNCTION);
    DEFINE_TOKENS
    #undef X


    return dictionary;
}

void expand_dictionary(tokentype_dictionary* dictionary){
    uint32_t max = dictionary->maximum_amount;
    uint32_t new_maximum_amount = max + DICTIONARY_EXPAND_AMOUNT;

    dictionary->dictionary = safe_realloc(dictionary->dictionary, (size_t) (sizeof(tokentype_dictionary_entry*)*new_maximum_amount));
    dictionary->free_list = safe_realloc(dictionary->free_list, (size_t) new_maximum_amount);

    safe_memset( dictionary->free_list + max, 0, (size_t) DICTIONARY_EXPAND_AMOUNT);

    dictionary->maximum_amount = new_maximum_amount;
}

/** initialises new type of token
 * 
 */
void create_new_tokentype(tokentype_dictionary* dictionary, char* target_lexeme, token_values value, token_types function){
    if(target_lexeme==NULL){
        return;
    }
    for(uint32_t i=0; i<dictionary->maximum_amount; ++i){
        if(dictionary->free_list[i] == 0){

            dictionary->free_list[i] = 1;
            tokentype_dictionary_entry* new_entry = (tokentype_dictionary_entry*)safe_malloc((size_t)sizeof(tokentype_dictionary_entry));
            new_entry->token_value = value;
            new_entry->token_type = function;
            new_entry->lexeme = (char*)safe_malloc((size_t)strlen(target_lexeme) + 1);
            strcpy(new_entry->lexeme, target_lexeme);

            dictionary->dictionary[i] = new_entry;

            return;
        }
    }
    //dictionary full - expand and attempt creation again
    expand_dictionary(dictionary);
    create_new_tokentype(dictionary, target_lexeme, value, function);
}

/** Used to acquire the TYPE of token associated with a lexeme, i.e., "+" is ADDITION token_value, with OPERATOR function.
 * 
 */
tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, char* target_lexeme){
    for(uint32_t i=0; i<dictionary->maximum_amount; ++i){
        //check if free_list is 1 and the actual entry is not null
        if(dictionary->free_list[i] == 1 && dictionary->dictionary[i] != NULL){
            if (!strcmp(dictionary->dictionary[i]->lexeme, target_lexeme)){
                return dictionary->dictionary[i];
            }
        }
    }
    return NULL;
}

/** Produces token - either searches dictionary and acquires pre-existing token details (Like operator type from its associated lexeme, or identifier)
 * 
 */
token* produce_token(token* prev, tokentype_dictionary* dictionary, lexeme* target_lexeme){
    token* new_token = (token*)safe_malloc((size_t)sizeof(token));
    new_token->next = NULL;
    new_token->leaf = false;
    if(prev != NULL){
        new_token->previous = (struct token*)prev;
        prev->next = (struct token*)new_token;
    }else{
        new_token->previous = NULL;
    }

    if(target_lexeme->type == STRING_LITERAL || target_lexeme->type == INT_VALUE){
        //int or string lexeme
        new_token->token_type = target_lexeme->type;
        new_token->token_value.variable_value = (void*)target_lexeme->value;
        new_token->leaf = true;
        return new_token;
    }else{
        tokentype_dictionary_entry* tokentype = tokentype_lookup(dictionary, target_lexeme->value);
        if(tokentype != NULL){
            //tokentype already exists, use existing tokentype data
            new_token->token_value = tokentype->token_value;
            new_token->token_type = tokentype->token_type;
            return new_token;
        }else{
            //tokentype does not exist - create a new tokentype
            token_values identifier_value;
            identifier_value.identifier_token_value = dictionary->identifier_count;
            create_new_tokentype(dictionary, target_lexeme->value, identifier_value, IDENTIFIER);
            new_token->token_value.variable_value = (void*)target_lexeme->value;
            new_token->token_type = IDENTIFIER;
            dictionary->identifier_count++;
            new_token->leaf = true;
            return new_token;
        }
    }
    return NULL;
}