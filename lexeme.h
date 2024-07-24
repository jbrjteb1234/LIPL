#ifndef lexer_buffer
#define lexer_buffer

typedef struct{
    int index;
    int length;
    char* buffer; 
} buffer;

void check_lexeme(buffer*, char*);

buffer* create_buffer();

void insert_to_buffer(buffer*, char lexeme_char);

#endif
