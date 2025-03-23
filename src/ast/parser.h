#ifndef PARSER
#define PARSER

#include "ast_utility/ast.h"
#include "table_management/table.h"

statement_list* parse(token** token);

void transfer_node_buffer(table_iterator* iterator);

#endif