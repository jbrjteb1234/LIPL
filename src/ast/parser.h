#ifndef LIPL_AST_PARSER_H
#define LIPL_AST_PARSER_H

#include "ast_utility/ast.h"
#include "../lexer/lexer_main.h"
#include "../lexer/symbol_table.h"

/* Parses a token stream into an AST.
   Syntax errors are printed to stderr as file:line:column with the line
   quoted; the parser recovers
   and keeps going so one run reports as many as it can. If there were any
   errors, NULL is returned. Otherwise the caller owns the tree and frees it
   with destroy_ast(). The tree doesn't point into the token stream, so the
   tokens can be destroyed straight after parsing. */
ast_root* parse(const token_stream* tokens, const symbol_table* symbols);

#endif /* LIPL_AST_PARSER_H */
