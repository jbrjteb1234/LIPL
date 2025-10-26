// main.c
#include <stdlib.h>
#include <stdio.h>
#include "lexer/lexer_main.h"
#include "ast/parser.h"

int main(int arg_count, char *args[]){

    if (arg_count != 2){
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }

    const char *filename = args[1];
    FILE *source_code = fopen(filename,"r");
    if (source_code == NULL){
        perror("Could not open file");
        return 1;
    }

    token* first_token = tokenize(source_code);

    /* parsing consumes token stream via pointer-to-pointer */
    token** first_token_address = &first_token;
    statement_list* ast_roots = parse(first_token_address);
    printf("AST: %p\n", (void*)ast_roots);

    /* cleanup */
    destroy_ast(ast_roots);
    destroy_token_stream(first_token);
    fclose(source_code);

    return 0;
}