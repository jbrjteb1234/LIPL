// main.c
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lexer/lexer_main.h"
#include "lexer/symbol_table.h"
#include "ast/parser.h"
#include "ast/ast_utility/ast_printer.h"

/* Exit codes: 0 = parsed fine, 1 = errors in the source (or unreadable file), 2 = bad command line */

static void print_usage(const char* program_name){
    fprintf(stderr,
            "usage: %s [--tokens] [--ast] <source-file>\n"
            "  --tokens   print the tokens\n"
            "  --ast      print the syntax tree (the default, until there's an interpreter)\n",
            program_name);
}

int main(int arg_count, char *args[]){
    const char* program_name = arg_count > 0 ? args[0] : "program";
    bool show_tokens = false;
    bool show_ast = false;
    const char* filename = NULL;

    for (int i = 1; i < arg_count; ++i){
        if (strcmp(args[i], "--tokens") == 0){
            show_tokens = true;
        }else if (strcmp(args[i], "--ast") == 0){
            show_ast = true;
        }else if (args[i][0] == '-' || filename != NULL){
            print_usage(program_name);
            return 2;
        }else{
            filename = args[i];
        }
    }
    if (filename == NULL){
        print_usage(program_name);
        return 2;
    }

    FILE *source_code = fopen(filename, "rb");
    if (source_code == NULL){
        perror(filename);
        return 1;
    }

    symbol_table* symbols = create_symbol_table();
    token_stream* tokens = tokenize(source_code, filename, symbols);
    fclose(source_code);
    if (tokens == NULL){
        fprintf(stderr, "%s: could not read file\n", filename);
        destroy_symbol_table(symbols);
        return 1;
    }

    if (show_tokens){
        print_token_stream(stdout, tokens, symbols);
    }

    int status = 0;
    if (tokens->error_count > 0){
        /* already reported; parsing broken tokens would only add noise */
        status = 1;
    }else if (show_ast || !show_tokens){
        ast_root* ast = parse(tokens, symbols);
        if (ast == NULL){
            status = 1;
        }else{
            print_ast(stdout, ast, symbols);
            destroy_ast(ast);
        }
    }

    /* cleanup */
    destroy_token_stream(tokens);
    destroy_symbol_table(symbols);
    return status;
}
