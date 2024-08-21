#ifndef lexer_info
#define lexer_info
#include "../grammar/grammar.h"
#include <stdbool.h>

//token structure - one command
typedef struct token{
    struct token* previous;
    struct token* next;
    
    unsigned char precedence;

    //Used later by the AST
    bool leaf;
    
    token_types     token_type;
    token_values    token_value;
} token;

#endif