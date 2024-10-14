#ifndef AST
#define AST

#include "../../grammar/grammar.h"
#include "../../lexer/token.h"

#include <stdint.h>

typedef struct ASTNode ASTNode;
typedef struct statement_list statement_list;

typedef enum{
    NONE_AST_TYPE,
    LEAF_NODE,
    BINARY_OP_NODE,
    RES_WORD_NODE,
    LIST_NODE,
    PARENTHESES_NODE,
}ASTNodeType;

typedef enum{
    ID_NODE,
    DEC_NODE,
    STR_NODE,
    INT_NODE,
}leaf_node;

typedef enum{
    ASSIGNMENT_NODE,
    ADDITION_NODE,
    SUBTRACTION_NODE,
    MULTIPLICATION_NODE,
    DIVISION_NODE,
}binary_op_node;

typedef enum{
    FUNCTION_DEC_NODE,
    FUNCTION_CALL_NODE,
    IF_NODE,
    ELSE_NODE,
    WHILE_NODE,
    FOR_NODE,
} res_node;

typedef union{
    leaf_node leaf_node_value;
    binary_op_node binary_op_node_value;
    res_node res_node_value;
} ASTNodeValue;

struct ASTNode{
    
    ASTNodeType type;
    ASTNodeValue value;

    union {
        union{
            void*   value;
            int     identifier;
        } value_node;

        struct {
            struct ASTNode* lhs;
            struct ASTNode* rhs;
        } binary_op_node;

        struct {
            struct ASTNode* identifier;
            struct ASTNode** arguments;
            uint32_t num_arguments;
        } function_node;
                   
        statement_list* list_node;

        // other node-specific data
    } data;

    struct ASTNode* parent;
};

struct statement_list{
    uint32_t max;
    uint32_t index;
    ASTNode** list;
};

void assign_ast_node_type(ASTNode* node, token** current_lookahead);

#endif