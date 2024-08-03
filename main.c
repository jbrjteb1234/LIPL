#include <stdlib.h>
#include <stdio.h>
#include "lexer/lexer_main.h"
#include "AST/ast.h"

int main(int arg_count, char *args[]){

    if (arg_count != 2){
        perror("Invalid arguments");
        return 1;
    }

    const char *filename = args[1];

    FILE *source_code = fopen(filename,"r");

    if (source_code == NULL){
        perror("Could not open file.");
        return 1;
    }

    token* first_token = tokenize(source_code);
    statement_list ast_roots = parse(first_token);


    return 0;
}