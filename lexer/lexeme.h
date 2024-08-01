#ifndef lexer_buffer
#define lexer_buffer

#include <stdbool.h>
#include "token.h"

typedef struct{
    int index;
    int length;

    char al_flag;
    char num_flag;
    char string_flag;


    char* buffer; 
} character_buffer;

character_buffer* create_character_buffer();

typedef struct{
    char* value;
    token_types type;
} lexeme;

void insert_to_character_buffer(character_buffer*, char lexeme_char);

bool produce_lexeme(character_buffer*, lexeme*, char);

#endif
