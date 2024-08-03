#include <stdio.h>
#include <stdlib.h>
#include "../utility/safe_memory.h"
#include "lexer_main.h"
#include "lexeme.h"
#include "token_type.h"
#include "token.h"

void print_token_stream(token* head) {
    token* current = head;
    while (current != NULL) {
        printf("Token: %d\n", current->token_type);
        current = (token*)current->previous;
    }
}
/** Turns source file into a linked list of tokens
 *  @return the head token - first command
 */
token* tokenize(FILE* source_code){
    int c;
    bool eof_flag = false;

    //character buffer - feeds characters from source file one by one into buffer 
    character_buffer* buf = create_character_buffer();
    //lexeme_buffer - holds complete lexeme to be turned into token
    lexeme* lexeme_buffer = malloc(sizeof(lexeme));

    token* current = NULL;
    token* first = NULL;
    tokentype_dictionary* dictionary = initialize_tokentype_dictionary();
    

    while (!eof_flag) {
        if((c = fgetc(source_code))==EOF){eof_flag = true;}
        if (produce_lexeme(buf, lexeme_buffer, c) == true){

            token* temp_new_token = produce_token(current, dictionary, lexeme_buffer);
            if (temp_new_token != NULL){
                current = temp_new_token;
                //set the head of the tokenstream
                if(first == NULL){
                    first = current;
                }
                //printf("New token created at address: %p, from lexeme %s\n", current, lexeme_buffer->value);
            }

            
        }
        insert_to_character_buffer(buf, c);
    }

    safe_free( (void**) &(lexeme_buffer->value) );
    safe_free( (void**) &lexeme_buffer );
    safe_free( (void**) &(buf->buffer));
    safe_free( (void**) &buf);
    //print_token_stream(current);

    return first;
}