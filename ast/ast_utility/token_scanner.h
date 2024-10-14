#ifndef TOKEN_SCANNER
#define TOKEN_SCANNER

#include "../../lexer/token.h"
#include "../tables/tables.h"
#include "ast.h"

void advance_token(token**);

ASTNode* push_token_into_ast_node(table_iterator* iterator, token** current_lookahead, bool auto_assign);

token* lookahead(token** scan_token);

#endif