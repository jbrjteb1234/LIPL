#ifndef AST
#define AST

#include "../grammar/grammar.h"
#include "../lexer/token.h"

typedef struct simplified_token{
    token_types token_type;
    token_values token_value;
} simplified_token;

typedef struct ASTNode{
    
    token* token;
    simplified_token simplified_token;

    union {
        struct {
            struct ASTNode* node_a;
            struct ASTNode* node_b;
        } binary_op_node;

        struct {
            struct ASTNode* node_a;
        } unary_op_node;

        struct {
            struct ASTNode** statements;
            int statement_count;
        } block_node;

        struct {
            struct ASTNode* condition;
            struct ASTNode* true_block;
            struct ASTNode* else_block;
        } condition_else_node;

        union{
            void* value;
        } value_node;

        // other node-specific data
    } data;


} ASTNode;

#define STATEMENT_LIST_INITIAL_SIZE 10

typedef ASTNode** statement_list;

ASTNode** parse(token* token);

#endif