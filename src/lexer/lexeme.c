// lexer/lexeme.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexeme.h"
#include "../utility/safe_memory.h"
#include "../grammar/grammar.h"

/* helper: is an unescaped quote at buffer[index-1]? */
static bool last_quote_is_unescaped(const character_buffer* buf){
    if (buf->index == 0) return false;
    if (buf->buffer[buf->index - 1] != '"') return false;
    /* count trailing backslashes before the quote */
    size_t k = 0;
    for (int64_t i = (int64_t)buf->index - 2; i >= 0 && buf->buffer[i] == '\\'; --i) {
        ++k;
    }
    return (k % 2u) == 0u; /* even count => quote not escaped */
}

/** creates and initialises character buffer - characters are fed from source file here */
character_buffer* create_character_buffer(void){
    character_buffer* b = (character_buffer*)safe_malloc(sizeof(*b));
    b->index = 0;
    b->length = 20;
    b->al_flag = 0;
    b->num_flag = 0;
    b->string_flag = 0;
    b->buffer = (char*)safe_malloc(b->length);
    return b;
}

/** inserts singular character into character buffer */
void insert_to_character_buffer(character_buffer* buf, char ch){
    if (buf->index == buf->length){
        uint32_t new_length = buf->length * 2u;
        buf->buffer = (char*)safe_realloc(buf->buffer, (size_t)new_length);
        buf->length = new_length; /* FIX: keep capacity in sync */
    }
    buf->buffer[buf->index++] = ch;
}

/** copy the contents of character buffer into lexeme */
void copy_buffer(character_buffer* buf, lexeme* out){
    char** dst = &out->value;
    if (*dst) {
        safe_free((void**)dst);
    }
    /* allocate space for NUL */
    *dst = (char*)safe_malloc((size_t)buf->index + 1u);
    safe_memcpy(*dst, buf->buffer, (size_t)buf->index);
    (*dst)[buf->index] = '\0';

    if (buf->al_flag) {
        out->type = IDENTIFIER;
    } else if (buf->num_flag) {
        out->type = INT_VALUE;
    } else {
        out->type = (token_types)-1;
    }
}

/** empty contents of buffer (resets flags) */
void empty_buffer(character_buffer* buf){
    buf->index = 0;
    buf->al_flag = 0;
    buf->num_flag = 0;
    buf->string_flag = 0;
}

/** Produce a lexeme from buffered chars given next input char (as int).
 * Returns true iff a lexeme is ready in `out` (caller must read it).
 */
bool produce_lexeme(character_buffer* buf, lexeme* out, int next_int){
    if (next_int == EOF){
        if (buf->index > 0){
            /* If we were inside a string, complain about unterminated string */
            if (buf->buffer[0] == '"' && !last_quote_is_unescaped(buf)){
                fprintf(stderr, "Unterminated string literal at EOF\n");
            }
            copy_buffer(buf, out);
            empty_buffer(buf);
            return true;
        }
        return false;
    }

    char next = (char)next_int;

    if (buf->index == 0){
        /* nothing to decide yet */
        return false;
    }

    char last = buf->buffer[buf->index - 1];
    char first = buf->buffer[0];

    if (isalpha((unsigned char)last)) {
        buf->al_flag = 1;
    } else if (isdigit((unsigned char)last)) {
        buf->num_flag = 1;
    }

    /* STRING MODE */
    if (first == '"'){
        if (last_quote_is_unescaped(buf)){
            /* we just closed the string with the last char */
            copy_buffer(buf, out);
            empty_buffer(buf);
            out->type = STRING_LITERAL;
            return true;
        }
        /* still inside string: keep accumulating regardless of next */
        return false;
    }

    /* if we are building whitespace, discard */
    if (isspace((unsigned char)last) || last == '\n'){
        empty_buffer(buf);
        return false;
    }

    /* If next is whitespace or newline, current lexeme ends */
    if (isspace((unsigned char)next) || next == '\n'){
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;
    }

    /* Multi-char operators: if last is one-char punct and next is '=' -> defer split */
    if (ispunct((unsigned char)next)){
        if (ispunct((unsigned char)last) && next == '='){
            return false; /* allow <=, >=, == to complete */
        }
        /* end current token before consuming this punctuation */
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;
    }

    /* If current lexeme is punctuation but next is alnum -> end current */
    if (isalnum((unsigned char)next) && ispunct((unsigned char)last)){
        copy_buffer(buf, out);
        empty_buffer(buf);
        return true;
    }

    /* otherwise, keep accumulating */
    return false;
}
