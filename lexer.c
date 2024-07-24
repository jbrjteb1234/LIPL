#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lexer.h"
#include "lexeme.h"

token* tokenize(FILE* source_code){
    char delimiters[2] = {' ', '\n'};

    //iterate through file, splitting it up into lexemes and tokenizing each lexeme
    int c;

    buffer* buf = create_buffer();

    while ((c = fgetc(source_code)) != EOF) {
        printf("%c", c);
    }

    //placeholder until tokenization is complete.
    token* a;
    return a;

}