#ifndef AST
#define AST

#include "../../grammar/grammar.h"
#include "../../lexer/token.h"

#include <stdint.h>

typedef struct ASTNode ASTNode;
typedef struct statement_list statement_list;

struct ASTNode{
    
    token* token;

    union {
        struct {
            struct ASTNode* lhs;
            struct ASTNode* rhs;
            operator_token  type;
        } binary_op_node;

        struct {
            struct ASTNode* identifier;
            struct ASTNode* value;
            
        } decl_node;

        struct {
            struct ASTNode* node_a;
        } unary_op_node;

        struct {
            struct ASTNode* condition;
            struct ASTNode* true_block;
            struct ASTNode* else_block;
        } condition_else_node;

        union{
            void*   value;
            int     identifier;
        } value_node;

        // other node-specific data
    } data;

    struct ASTNode* parent;
    char statement_list_node;
    bool leaf_node;

};

struct statement_list{
    uint32_t max;
    uint32_t index;
    ASTNode** list;
};

#endif