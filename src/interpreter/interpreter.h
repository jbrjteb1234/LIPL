#ifndef LIPL_INTERPRETER_H
#define LIPL_INTERPRETER_H

#include "../ast/ast_utility/ast.h"
#include "../lexer/symbol_table.h"
#include "../utility/source_file.h"

/* Runs a parsed program, walking the AST directly.
   Built-in function names are added to `symbols`. A runtime error is printed
   to stderr as file:line:column, quoting the line from `source`, and stops
   the program. Returns 0 if the program ran to the end, 1 after a runtime error. */
int interpret(const ast_root* ast, symbol_table* symbols, const source_file* source);

#endif /* LIPL_INTERPRETER_H */
