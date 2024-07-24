#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexeme.h"

buffer* create_buffer(){
    buffer* new_buffer = malloc(sizeof(buffer));
    new_buffer->index = 0;
    new_buffer->length = 10;
    new_buffer->buffer = malloc(new_buffer->length);
    return new_buffer;
}

void insert_to_buffer(buffer* buf, char lexeme_char){
    if (buf->index == buf->length){
        int new_length = buf->length + 10;
        buf->buffer = realloc(buf->buffer, new_length);
    }
        buf->buffer[buf->index] = lexeme_char;
}

void check_lexeme(buffer* buf, char* out){

}