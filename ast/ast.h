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

    union {
        struct {
            struct ASTNode* node_a;
            struct ASTNode* node_b;
        } binary_op_node;

        struct {
            struct ASTNode* node_a;
        } unary_op_node;

        struct {
            statement_list statements;
            int statement_count;
        } statement_list_node;

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

    struct ASTNode* parent;
    char statement_list_node;

} ASTNode;

#define STATEMENT_LIST_INITIAL_SIZE 10
#define SUB_STATEMENT_LIST_INITIAL_SIZE 5

//statement list - where root nodes get added to for execution
typedef struct {
    ASTNode** list;
    struct statement_list* parent;
    int index;
} statement_list;

ASTNode** parse(token* token);

#endif