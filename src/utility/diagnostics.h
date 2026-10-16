#ifndef LIPL_UTILITY_DIAGNOSTICS_H
#define LIPL_UTILITY_DIAGNOSTICS_H

#include <stdarg.h>
#include <stdint.h>
#include "source_file.h"

/* Prints an error to stderr the way clang and rustc do, quoting the source
   line with a caret under the column:

       fizz.lipl:3:10: error: expected ';' after expression, found identifier 'y'
           3 | x = 1 + 2
             |          ^

   `line` and `column` are 1-based, and the column counts bytes, as the lexer
   does. "error:" is red and the caret green when stderr is a terminal (and
   NO_COLOR isn't set); otherwise the output is plain text. */
void report_error(const source_file* source, uint32_t line, uint32_t column, const char* format, va_list args);

#endif /* LIPL_UTILITY_DIAGNOSTICS_H */
