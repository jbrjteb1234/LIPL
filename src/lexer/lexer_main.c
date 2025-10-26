// lexer/lexer_main.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utility/safe_memory.h"
#include "../utility/data_pool.h"
#include "lexer_main.h"
#include "lexeme.h"
#include "token_type.h"
#include "token_dictionary.h"
#include "token.h"

static data_pool* g_token_pool = NULL; /* the pool backing token nodes */

void print_token_stream(token* head) {
    for (token* t = head; t != NULL; t = (token*)t->next) {
        printf("Token: %d\n", t->token_type);
    }
}

/* NOTE: tokenize() creates and destroys the dictionary internally;
         tokens do NOT keep any pointers into the dictionary. */
token* tokenize(FILE* source_code){
    int c;
    bool eof_flag = false;

    character_buffer* buf = create_character_buffer();
    lexeme* lexeme_buffer = (lexeme*)safe_malloc(sizeof(lexeme));
    lexeme_buffer->value = NULL;
    lexeme_buffer->type = (token_types)-1;

    token* current = NULL;
    token* first = NULL;

    tokentype_dictionary* dictionary = initialize_tokentype_dictionary();

    g_token_pool = init_data_pool((uint32_t)sizeof(token), 50u);

    while (!eof_flag) {
        if((c = fgetc(source_code))==EOF){eof_flag = true;}
        if (produce_lexeme(buf, lexeme_buffer, c)){

            token* temp_new_token = produce_token(current, dictionary, lexeme_buffer, g_token_pool);
            if (temp_new_token != NULL){
                current = temp_new_token;
                if(first == NULL){
                    first = current;
                }
                /* optional trace */
                /* printf("New token created at %p from lexeme %s\n", (void*)current, lexeme_buffer->value); */
            }

            /* transfer of ownership rules:
               - STRING_LITERAL / INT_VALUE: token made a deep copy; free the lexeme buffer here.
               - IDENTIFIER / operators / reserved words: dictionary copied strings internally;
                 free the lexeme buffer here as well. */
            if (lexeme_buffer->value){
                safe_free((void**)&lexeme_buffer->value);
            }
        }
        if (!eof_flag){
            insert_to_character_buffer(buf, (char)c);
        }
    }

    destroy_tokentype_dictionary(dictionary);

    /* cleanup temporary buffers */
    if (lexeme_buffer->value) safe_free((void**)&lexeme_buffer->value);
    safe_free((void**)&lexeme_buffer);
    safe_free((void**)&buf->buffer);
    safe_free((void**)&buf);

    return first;
}

void destroy_token_stream(token* head){
    /* free per-token owned payloads */
    for (token* t = head; t; t = (token*)t->next){
        if (t->leaf && (t->token_type == STRING_LITERAL || t->token_type == INT_VALUE)){
            if (t->token_value.variable_value){
                safe_free((void**)&t->token_value.variable_value);
            }
        }
    }
    /* release the token node pool */
    if (g_token_pool){
        shutdown_data_pool(g_token_pool);
        g_token_pool = NULL;
    }
}