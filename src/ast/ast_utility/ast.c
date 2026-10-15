#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "slist_functions.h"
#include "../../utility/safe_memory.h"

#define INITIAL_NODE_POOL_SIZE 64u

ast_root* create_ast_root(void){
    ast_root* root = (ast_root*)safe_malloc(sizeof(ast_root));
    root->node_pool     = init_data_pool(sizeof(ASTNode), INITIAL_NODE_POOL_SIZE);
    root->owned_lists   = create_stack(sizeof(statement_list*));
    root->owned_strings = create_stack(sizeof(char*));
    root->statements    = new_ast_list(root);
    return root;
}

ASTNode* new_ast_node(ast_root* root, ASTNodeType type, uint32_t line, uint32_t column){
    ASTNode* node = (ASTNode*)acquire_from_pool(root->node_pool);
    node->type = type;
    node->line = line;
    node->column = column;
    return node;
}

statement_list* new_ast_list(ast_root* root){
    statement_list* list = create_new_slist();
    push(root->owned_lists, list, false);
    return list;
}

char* copy_ast_string(ast_root* root, const char* text){
    char* copy = safe_strndup(text, strlen(text));
    push(root->owned_strings, copy, false);
    return copy;
}

void destroy_ast(ast_root* root){
    if (!root) return;

    void* item;
    while ((item = pop(root->owned_lists)) != NULL){
        statement_list* list;
        memcpy(&list, item, sizeof list);
        destroy_slist(list);
    }
    while ((item = pop(root->owned_strings)) != NULL){
        char* text;
        memcpy(&text, item, sizeof text);
        safe_free((void**)&text);
    }

    destroy_stack(root->owned_lists);
    destroy_stack(root->owned_strings);
    shutdown_data_pool(root->node_pool);
    safe_free((void**)&root);
}
