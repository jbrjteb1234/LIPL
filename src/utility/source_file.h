#ifndef LIPL_UTILITY_SOURCE_FILE_H
#define LIPL_UTILITY_SOURCE_FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* A program's text, kept in memory for the whole run so that errors from the
   lexer, the parser and the interpreter can all quote the line they're about. */
typedef struct source_file{
    const char* name;       /* borrowed: the path as given, used in error messages */
    char*       text;       /* owned; NUL-terminated, but may also contain NULs */
    size_t      length;
} source_file;

/* Reads all of `file` into `out`. Returns false if reading failed. */
bool read_source_file(FILE* file, const char* name, source_file* out);

void free_source_file(source_file* source);

#endif /* LIPL_UTILITY_SOURCE_FILE_H */
