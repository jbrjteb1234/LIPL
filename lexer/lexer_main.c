#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lexer_main.h"
#include "lexeme.h"

token* tokenize(FILE* source_code){
    char delimiters[2] = {' ', '\n'};

    //iterate through file, splitting it up into lexemes and tokenizing each lexeme
    int c;

    lbuffer* buf = create_lbuffer();

    token* prev = NULL;
    token* new = NULL;

    while ((c = fgetc(source_code)) != EOF) {
        printf("%c", c);
    }

    //placeholder until tokenization is complete.
    token* a;
    return a;
}