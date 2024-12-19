#include "ast.h"
#include "../table_management/table.h"

void return_to_previous_state(table_iterator* iterator);

void advance_token(token** scan_token);

token* lookahead(token** scan_token);

void attach_to_previous_conditional_block(table_iterator* iterator, ASTNode* new_block_node);

void transfer_node_buffer(table_iterator* iterator);