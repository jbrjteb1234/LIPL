// main.c
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lexer/lexer_main.h"
#include "lexer/symbol_table.h"
#include "ast/parser.h"
#include "ast/ast_utility/ast_printer.h"
#include "interpreter/interpreter.h"
#include "utility/source_file.h"

/* Exit codes: 0 = ran (or parsed) fine, 1 = an error in the program (or an unreadable file),
   2 = bad command line */

#define LIPL_VERSION "1.0.0"

static void print_usage(FILE* out){
    fputs("usage: lipl [--tokens] [--ast] <source-file>\n"
          "  with no options, runs the program\n"
          "  --tokens    print the tokens instead of running\n"
          "  --ast       print the syntax tree instead of running\n"
          "  --help      show this message\n"
          "  --version   show the version number\n",
          out);
}

int main(int arg_count, char *args[]){
    bool show_tokens = false;
    bool show_ast = false;
    const char* filename = NULL;

    for (int i = 1; i < arg_count; ++i){
        if (strcmp(args[i], "--help") == 0){
            print_usage(stdout);
            return 0;
        }else if (strcmp(args[i], "--version") == 0){
            puts("lipl " LIPL_VERSION);
            return 0;
        }else if (strcmp(args[i], "--tokens") == 0){
            show_tokens = true;
        }else if (strcmp(args[i], "--ast") == 0){
            show_ast = true;
        }else if (args[i][0] == '-'){
            fprintf(stderr, "lipl: unknown option '%s'\n", args[i]);
            print_usage(stderr);
            return 2;
        }else if (filename != NULL){
            fprintf(stderr, "lipl: one source file at a time ('%s' and '%s' were given)\n", filename, args[i]);
            print_usage(stderr);
            return 2;
        }else{
            filename = args[i];
        }
    }
    if (filename == NULL){
        print_usage(stderr);
        return 2;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL){
        perror(filename);
        return 1;
    }

    /* kept until the end so every stage's errors can quote the line they're about */
    source_file source;
    bool read_ok = read_source_file(file, filename, &source);
    fclose(file);
    if (!read_ok){
        fprintf(stderr, "%s: could not read file\n", filename);
        return 1;
    }

    symbol_table* symbols = create_symbol_table();
    token_stream* tokens = tokenize(&source, symbols);

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
            if (show_ast){
                print_ast(stdout, ast, symbols);
            }else{
                status = interpret(ast, symbols, &source);
            }
            destroy_ast(ast);
        }
    }

    /* cleanup */
    destroy_token_stream(tokens);
    destroy_symbol_table(symbols);
    free_source_file(&source);
    return status;
}
