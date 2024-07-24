#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef lexer_info
#define lexer_info

typedef enum {
    RESERVED_WORD,
    OPERATOR,
    STRING_LITERAL,
    NUM,
    IDENTIFIER
} token_functions;

typedef enum{
    SUBTRACT,
    ADDITION,
    MULTIPLY,
    EQUIVALENT,
    OR
} operator_token;

typedef enum{
    INT,
    IF,
    ELSE
} reserved_word_token;

//The token value - could be an enum for operators or reserved words, or a variable value
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    int*                    number_token_value;
    char *                  string_literal_token_value;
    char *                  identifier_token_value;                 
} token_values;

typedef struct{
    struct token* previous;
    struct token* next;
    
    token_functions token_type;
    token_values    token_value;
} token;

token* tokenize(FILE *);

#endif