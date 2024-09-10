#ifndef lexer_buffer
#define lexer_buffer

#include <stdbool.h>
#include "token.h"
#include <stdint.h>

typedef struct{
    uint32_t index;
    uint32_t length;

    char al_flag;
    char num_flag;
    char string_flag;


    char* buffer; 
} character_buffer;

typedef struct{
    char* value;
    token_types type;
} lexeme;

character_buffer* create_character_buffer(void);

void insert_to_character_buffer(character_buffer*, char lexeme_char);

void copy_buffer(character_buffer* buf, lexeme* lexeme);

void empty_buffer(character_buffer* buf);

bool produce_lexeme(character_buffer*, lexeme*, int);

#endif
