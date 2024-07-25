#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexeme.h"

lbuffer* create_lbuffer(){
    lbuffer* new_lbuffer = malloc(sizeof(lbuffer));
    new_lbuffer->index = 0;
    new_lbuffer->length = 10;
    new_lbuffer->buffer = malloc(new_lbuffer->length);
    return new_lbuffer;
}

void insert_to_lbuffer(lbuffer* buf, char lexeme_char){
    if (buf->index == buf->length){
        int new_length = buf->length + 10;
        buf->buffer = realloc(buf->buffer, new_length);
    }
        buf->buffer[buf->index] = lexeme_char;
}

void check_lexeme(lbuffer* buf, char* out){
    char delimiters[2] = {' ', '\n'};

    

}