#ifndef LIPL_LEXER_MAIN_H
#define LIPL_LEXER_MAIN_H

#include <stdio.h>
#include <stddef.h>
#include "token.h"
#include "symbol_table.h"
#include "../utility/data_pool.h"

/* The tokens of one source file: a doubly linked list that always ends with
   an END_OF_INPUT token, so the parser never has to check for NULL. */
typedef struct token_stream{
    token*      head;
    token*      tail;
    data_pool*  pool;           /* storage for every token node */
    const char* source_name;    /* borrowed; used in error messages */
    uint32_t    error_count;    /* lexical errors reported while tokenizing */
} token_stream;

/* Reads all of `source` and tokenizes it. Identifier names are interned into
   `symbols`. Lexical errors are printed to stderr as file:line:column and
   counted in error_count; the stream is still returned so callers can decide
   what to do. Returns NULL only if the file could not be read. */
token_stream* tokenize(FILE* source, const char* source_name, symbol_table* symbols);

void destroy_token_stream(token_stream* stream);

void print_token_stream(FILE* out, const token_stream* stream, const symbol_table* symbols);

/* Source spelling of an operator, delimiter or reserved word ("+", "{", "while"), or NULL. */
const char* fixed_lexeme(token_types type, token_values value);

const char* operator_lexeme(operator_token op);

/* Human-readable description of a token for error messages, e.g. "identifier 'x'". */
void describe_token(const token* t, const symbol_table* symbols, char* buffer, size_t size);

/* Writes a string with quotes and escapes, the way it would appear in source. */
void print_string_literal(FILE* out, const char* text);

/* Formats a decimal so it always reads as one ("2.0", not "2"). */
void format_float_literal(double value, char* buffer, size_t size);

#endif /* LIPL_LEXER_MAIN_H */
