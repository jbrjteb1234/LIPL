#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "parser.h"
#include "ast_utility/ast.h"
#include "../utility/safe_memory.h"
#include "states.h"
#include "../utility/stack.h"
#include "ast_utility/slist_functions.h"
#include "ast_utility/routines.h"
#include "table_management/token_converter.h"

/* Must match the sentinel used in slist_functions.c */
#define SLIST_EMPTY_INDEX 0xFFFFFFFFu

/** push staged node (from convert_token) if present */
void transfer_node_buffer(table_iterator* iterator) {
    if (iterator->new_node_buffer_set_flag) {
        push(iterator->node_stack, iterator->new_node_buffer, false);
    }
    iterator->new_node_buffer_set_flag = false;
}

static void set_block_pool(statement_list* sl, table_iterator* it) {
    if (sl) sl->backing_pool = it->node_pool;
}

statement_list* parse(token** scan_token) {

    statement_list* global_slist = create_new_slist();

    table_iterator* iterator = initialize_table_iterator(global_slist);
    set_block_pool(global_slist, iterator);

    stack* working_list_stack = create_stack(sizeof(statement_list*));

    shift_results result;
    bool final_token = false;

    while (!final_token) {
        result = shift(iterator, scan_token);
        switch (result) {
            case ERROR:
                fprintf(stderr, "Error in parsing\n");
                return NULL;

            case HOLD:
                /* hold same token, shift again */
                break;

            case ADVANCE:
                transfer_node_buffer(iterator);
                advance_token(scan_token);
                break;

            case FINISH: {
                ASTNode* new_statement = close_iterator(iterator);
                append_to_slist(iterator->working_list, new_statement);

                if ((*scan_token)->next == NULL) {
                    final_token = true;
                } else {
                    advance_token(scan_token);
                }
                break;
            }

            case OPEN_BLOCK: {
                ASTNode* control_statement = close_iterator(iterator);
                if (control_statement->block_flag == false) {
                    fprintf(stderr, "Tried to open block on non-block node\n");
                    return NULL;
                }

                append_to_slist(iterator->working_list, control_statement);
                statement_list* new_block = create_new_slist();
                set_block_pool(new_block, iterator);

                push(working_list_stack, iterator->working_list, false);
                control_statement->block = new_block;
                iterator->working_list   = new_block;

                advance_token(scan_token);
                break;
            }

            case CLOSE_BLOCK: {
                if ((*scan_token)->next == NULL) {
                    final_token = true;
                } else {
                    advance_token(scan_token);
                }
                if (working_list_stack->top == -1) {
                    fprintf(stderr, "Tried to close block without opening one\n");
                    return NULL;
                }
                iterator->working_list = *(statement_list**)pop(working_list_stack);
                break;
            }
        }
    }

    return global_slist;
}

/* free only the statement_list containers; nodes come from a pool */
static void destroy_ast_internal(statement_list* root) {
    if (!root) return;

    /* If the list is non-empty, traverse all elements 0..index inclusive. */
    if (root->index != SLIST_EMPTY_INDEX) {
        for (uint32_t i = 0; i <= root->index; ++i) {
            ASTNode* n = root->list[i];
            if (!n) continue;

            /* Recurse into block bodies (if/while/else/func, etc.). */
            if (n->block) {
                destroy_ast_internal(n->block);
            }

            /* Also recurse into LIST_NODE containers (argument / expression lists). */
            if (n->type == LIST_NODE && n->data.list_node) {
                destroy_ast_internal(n->data.list_node);
            }
        }
    }

    if (root->list) {
        safe_free((void**)&root->list);
    }
    safe_free((void**)&root);
}

void destroy_ast(statement_list* root) {
    if (!root) return;

    /* All AST nodes were allocated from a single pool recorded on the root. */
    data_pool* pool = root->backing_pool;

    destroy_ast_internal(root);

    if (pool) {
        shutdown_data_pool(pool);
    }
}
