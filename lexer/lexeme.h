#ifndef lexer_buffer
#define lexer_buffer
#include <stdbool.h>

typedef struct{
    int index;
    int length;
    char* buffer; 
} character_buffer;

character_buffer* create_character_buffer();

void insert_to_character_buffer(character_buffer*, char lexeme_char);

bool produce_lexeme(character_buffer*, char**, char);

#endif
