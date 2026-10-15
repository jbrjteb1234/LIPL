#ifndef LIPL_AST_PRINTER_H
#define LIPL_AST_PRINTER_H

#include <stdio.h>
#include "ast.h"
#include "../../lexer/symbol_table.h"

/* Prints the tree as an indented outline, one node per line. */
void print_ast(FILE* out, const ast_root* root, const symbol_table* symbols);

#endif /* LIPL_AST_PRINTER_H */
