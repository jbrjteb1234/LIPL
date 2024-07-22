#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef lexer_info
#define lexer_info

typedef struct{
    struct token* previous;
    struct token* next;
    uint16_t identifier;
} token;

token* tokenize(FILE *);

#endif