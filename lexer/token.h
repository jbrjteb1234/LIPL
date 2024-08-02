#ifndef lexer_info
#define lexer_info
#include "../grammar/grammar.h"

//The token value - could be an enum for operators or reserved words, or a variable value
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    int*                    number_token_value;
    float*                  float_token_value;
    char *                  string_literal_token_value;
    int                     identifier_token_value;                 
} token_values;

//token structure - one command
typedef struct{
    struct token* previous;
    struct token* next;
    
    unsigned char precedence;
    
    token_types     token_type;
    token_values    token_value;
} token;

#endif