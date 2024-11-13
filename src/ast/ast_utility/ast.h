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
    FUNC_NODE,
    LIST_NODE,
    PARENTHESES_NODE,
    CONDITIONAL_BLOCK_NODE,
}ASTNodeType;

typedef enum{
    ID_NODE,
    DEC_NODE,
    STR_NODE,
    INT_NODE,
}leaf_node;

typedef enum{
    FUNC_CALL_NODE,
    FUNC_DEC_NODE,
} func_node;

typedef enum{
    ASSIGNMENT_NODE,
    ADDITION_NODE,
    SUBTRACTION_NODE,
    MULTIPLICATION_NODE,
    DIVISION_NODE,
}binary_op_node;

typedef enum{
    IF_NODE,
    ELSE_NODE,
    WHILE_NODE,
    FOR_NODE,
    ELIF_NODE,
} conditional_block_node;

typedef enum{
    RETURN_NODE,
} res_word_node;

typedef union{
    leaf_node leaf_node_value;
    binary_op_node binary_op_node_value;
    conditional_block_node conditional_block_node_value;
    func_node func_node_value;
    res_word_node reserved_word_value;
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
            int identifier;
            uint32_t arguments_count;
            struct ASTNode* arguments_list;
        } function_node;

        struct {
            struct ASTNode* condition;
            struct ASTNode* alternate;
        } conditional_block_node;
                   
        statement_list* list_node;

        // other node-specific data
    } data;

    struct ASTNode* parent;
    bool reduced;
    uint32_t specifiers;
    bool block_flag;
    statement_list* block;
};

struct statement_list{
    uint32_t max;
    uint32_t index;
    ASTNode** list;
};

void assign_ast_node_type(ASTNode* node, token** current_lookahead);

#endif