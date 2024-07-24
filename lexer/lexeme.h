#ifndef lexer_buffer
#define lexer_buffer

typedef struct{
    int index;
    int length;
    char* buffer; 
} lbuffer;

void check_lexeme(lbuffer*, char*);

lbuffer* create_lbuffer();

void insert_to_lbuffer(lbuffer*, char lexeme_char);

#endif
