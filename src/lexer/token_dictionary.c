// lexer/token_dictionary.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token_dictionary.h"
#include "token.h"
#include "../utility/safe_memory.h"
#include "../grammar/grammar.h"

#define INITIAL_DICTIONARY_SIZE 50
#define DICTIONARY_EXPAND_AMOUNT 10

static char* dup_cstr(const char* s){
    if (!s) return NULL;
    size_t n = strlen(s) + 1u;
    char* out = (char*)safe_malloc(n);
    safe_memcpy(out, s, n);
    return out;
}

tokentype_dictionary* initialize_tokentype_dictionary(void){
    tokentype_dictionary* dictionary = (tokentype_dictionary*)safe_malloc(sizeof(*dictionary));
    dictionary->maximum_amount = INITIAL_DICTIONARY_SIZE;
    dictionary->identifier_count = 0;

    dictionary->dictionary = (tokentype_dictionary_entry**)safe_malloc(
        sizeof(tokentype_dictionary_entry*) * dictionary->maximum_amount);
    safe_memset(dictionary->dictionary, 0, sizeof(tokentype_dictionary_entry*) * dictionary->maximum_amount);

    dictionary->free_list = (char*)safe_malloc(dictionary->maximum_amount);
    safe_memset(dictionary->free_list, 0, dictionary->maximum_amount);

    /* Preload all static tokens from grammar */
    #define X(LEXEME, VALUE, VALUE_TYPE, TYPE) \
        create_new_tokentype(dictionary, LEXEME, (token_values){ .VALUE_TYPE = VALUE }, TYPE);
    DEFINE_TOKENS
    #undef X

    return dictionary;
}

void destroy_tokentype_dictionary(tokentype_dictionary* dictionary){
    if (!dictionary) return;
    if (dictionary->dictionary){
        for (uint32_t i = 0; i < dictionary->maximum_amount; ++i){
            if (dictionary->free_list && dictionary->free_list[i] == 1 && dictionary->dictionary[i]){
                tokentype_dictionary_entry* e = dictionary->dictionary[i];
                if (e->lexeme) safe_free((void**)&e->lexeme);
                safe_free((void**)&dictionary->dictionary[i]); /* entry itself */
            }
        }
        safe_free((void**)&dictionary->dictionary);
    }
    if (dictionary->free_list) safe_free((void**)&dictionary->free_list);
    safe_free((void**)&dictionary);
}

void expand_dictionary(tokentype_dictionary* dictionary){
    uint32_t max = dictionary->maximum_amount;
    uint32_t new_max = max + DICTIONARY_EXPAND_AMOUNT;

    dictionary->dictionary = (tokentype_dictionary_entry**)safe_realloc(
        dictionary->dictionary, sizeof(tokentype_dictionary_entry*) * new_max);
    /* zero the new slots */
    safe_memset(dictionary->dictionary + max, 0, sizeof(tokentype_dictionary_entry*) * (new_max - max));

    dictionary->free_list = (char*)safe_realloc(dictionary->free_list, new_max);
    safe_memset(dictionary->free_list + max, 0, (size_t)(new_max - max));

    dictionary->maximum_amount = new_max;
}

void create_new_tokentype(tokentype_dictionary* dictionary, const char* target_lexeme, token_values value, token_types type){
    if (!target_lexeme) return;

    for (uint32_t i = 0; i < dictionary->maximum_amount; ++i){
        if (dictionary->free_list[i] == 0){
            dictionary->free_list[i] = 1;
            tokentype_dictionary_entry* e = (tokentype_dictionary_entry*)safe_malloc(sizeof(*e));
            e->token_value = value;
            e->token_type = type;
            e->lexeme = dup_cstr(target_lexeme);
            dictionary->dictionary[i] = e;
            return;
        }
    }
    /* no room — grow and retry */
    expand_dictionary(dictionary);
    create_new_tokentype(dictionary, target_lexeme, value, type);
}

tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, const char* target_lexeme){
    if (!target_lexeme) return NULL;
    for (uint32_t i = 0; i < dictionary->maximum_amount; ++i){
        if (dictionary->free_list[i] == 1 && dictionary->dictionary[i]){
            if (dictionary->dictionary[i]->lexeme && strcmp(dictionary->dictionary[i]->lexeme, target_lexeme) == 0){
                return dictionary->dictionary[i];
            }
        }
    }
    return NULL;
}

static token* make_token_link(token* prev, data_pool* pool){
    token* t = (token*)acquire_from_pool(pool);
    t->next = NULL;
    t->leaf = false;
    if (prev){
        t->previous = (struct token*)prev;
        prev->next = (struct token*)t;
    }else{
        t->previous = NULL;
    }
    return t;
}

token* produce_token(token* prev, tokentype_dictionary* dictionary, lexeme* target_lexeme, data_pool* token_pool){
    if (!target_lexeme) return NULL;

    token* new_token = make_token_link(prev, token_pool);

    if (target_lexeme->type == STRING_LITERAL){
        /* own a deep copy */
        new_token->token_type = STRING_LITERAL;
        new_token->token_value.variable_value = dup_cstr(target_lexeme->value);
        new_token->leaf = true;
        return new_token;
    }

    if (target_lexeme->type == INT_VALUE){
        /* keep as string for now; downstream can parse; still deep copy it */
        new_token->token_type = INT_VALUE;
        new_token->token_value.variable_value = dup_cstr(target_lexeme->value);
        new_token->leaf = true;
        return new_token;
    }

    /* operator / reserved / delimiter / identifier */
    tokentype_dictionary_entry* entry = tokentype_lookup(dictionary, target_lexeme->value);
    if (entry){
        /* static entries: operators, delimiters, reserved, or pre-declared identifiers */
        new_token->token_value = entry->token_value;
        new_token->token_type = entry->token_type;
        return new_token;
    }

    /* new IDENTIFIER: assign a stable integer ID and DO NOT store a string in the token */
    token_values idv;
    idv.identifier_token_value = dictionary->identifier_count++;
    create_new_tokentype(dictionary, target_lexeme->value, idv, IDENTIFIER);

    new_token->token_type = IDENTIFIER;
    new_token->token_value.identifier_token_value = idv.identifier_token_value;
    new_token->leaf = true;
    return new_token;
}