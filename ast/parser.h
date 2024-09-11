#ifndef PARSER
#define PARSER

#include "ast_utility/ast.h"

statement_list* parse(token* token);

void advance_token(token** scan_token);

#endif