#ifndef LEXER_MAIN
#define LEXER_MAIN

#include "token.h"
#include <stdio.h>

token* tokenize(FILE*);

void print_token_stream(token* head);

#endif