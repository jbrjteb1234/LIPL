#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexeme.h"
#include "../utility/safe_memory.h"

/** creates and initialises character buffer - characters are fed from source file here
 *  @return character_buffer* 
 */
character_buffer* create_character_buffer(){
    character_buffer* new_character_buffer = safe_malloc(sizeof(character_buffer));
    new_character_buffer->index = 0;
    new_character_buffer->length = 20;
    new_character_buffer->buffer = safe_malloc(new_character_buffer->length);
    return new_character_buffer;
}

/** inserts singular character into character buffer
 *  
 */
void insert_to_character_buffer(character_buffer* buf, int lexeme_char){
    if (buf->index == buf->length){
        int new_length = buf->length + 10;
        buf->buffer = safe_realloc(buf->buffer, new_length);
    }
    buf->buffer[buf->index] = lexeme_char;
    ++buf->index;
}

/** copy the contents of character buffer into lexer
 * 
 */
void copy_buffer(character_buffer* buf, lexeme* lexeme){

    char** out_p = &(lexeme->value);

    if(*out_p != NULL){
        safe_free((void**)&(*out_p));
    }
    *out_p = (char*)safe_malloc( buf ->index );
    safe_memcpy(*out_p, buf->buffer, buf->index);
    (*out_p)[buf->index] = '\0';

    if(buf->al_flag){
        //alpha, identifier
        lexeme->type = IDENTIFIER;
        return;
    }else if (buf->num_flag){
        //numbers, no alpha (int)
        lexeme->type = INT_VALUE;
        return;
    }else{
        lexeme->type = -1;
    }
}

/** empty contents of buffer (resets index to 0)
 * 
 */
void empty_buffer(character_buffer* buf){
    buf->index=0;
    buf->al_flag = 0;
    buf->num_flag = 0;
    buf->string_flag = 0;
}

/** Produces a lexeme, given a buffer filled with character straight from the source code
 *  @return boolean - whether or not a lexeme was produced
 */
bool produce_lexeme(character_buffer* buf, lexeme* out, int next){

    

    //the next char is alphanum, keep filling buffer
    if (buf->index == 0){
        return false;
    }

    char last_val = buf->buffer[buf->index-1];
    char first_val = buf->buffer[0];

    //check if a alpha or number has been passed into the buffer - useful later for determining what type token the lexeme should form
    if(isalpha(last_val)){
        buf->al_flag = 1;
    }else if (isdigit(last_val)){
        buf->num_flag = 1;
    }
    

    //string literal - ignore all until closed
    if (first_val == '"'){
        if (last_val == '"' && buf->index>1){
            copy_buffer(buf, out);
            empty_buffer(buf);
            out->type = STRING_LITERAL;
            return true;
        }
        //open string literal - keep filling buffer
        return false;

    //space or new-line in the buffer. ignore
    }else if (isspace(last_val) || last_val == '\n'){
        empty_buffer(buf);
        return false;

    //the next char is a space, newline, or EOF.
    } else if (isspace(next) || next == '\n' || next == EOF){
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;

    //the next char is a symbol. unless last char is symbol (could be multi-char symb like '=='), produce lexeme
    }else if (ispunct(next)){
        if (ispunct(last_val)){
            return false;
        }
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;

    //the next char is alphanum, but buffer holds symbols
    }else if (isalnum(next) && ispunct(last_val)){
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;
    }
}