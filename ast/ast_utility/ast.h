#ifndef AST
#define AST

#include "../../grammar/grammar.h"
#include "../../lexer/token.h"

#include <stdint.h>

typedef struct ASTNode ASTNode;
typedef struct statement_list statement_list;

typedef enum{
    VALUE_NODE,
    IDENTIFIER_NODE,
    BINARY_OP_NODE,
    ASSIGNMENT_NODE,
    DECLARATION_NODE,
    FUNCTION_DECLARATION_NODE,
    FUNCTION_CALL_NODE,
}ASTNodeType;

struct ASTNode{
    
    token* token;
    ASTNodeType type;

    union {
        union{
            void*   value;
            int     identifier;
        } value_node;

        struct {
            struct ASTNode* lhs;
            struct ASTNode* rhs;
            operator_token  type;
        } binary_op_node;

        struct {
            struct ASTNode* identifier;
            struct ASTNode* value;
        } assignment_node;

        struct {
            struct ASTNode* identifier;
            struct ASTNode** arguments;
            uint32_t num_arguments;
        } function_node;

        // other node-specific data
    } data;

    struct ASTNode* parent;
    bool leaf_node;

};

struct statement_list{
    uint32_t max;
    uint32_t index;
    ASTNode** list;
};

#endif