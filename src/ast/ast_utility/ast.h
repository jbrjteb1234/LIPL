#ifndef AST
#define AST

#include "../../grammar/grammar.h"
#include "../../lexer/token.h"

#include <stdint.h>

typedef uint16_t specifiers;
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
    ADDITION_NODE           =   ADDITION,
    SUBTRACTION_NODE        =   SUBTRACTION,
    MULTIPLICATION_NODE     =   MULTIPLICATION,
    DIVISION_NODE           =   DIVISION,
    ASSIGNMENT_NODE         =   ASSIGNMENT,
    EQUIVALENT_NODE         =   EQUIVALENT,
    NOT_EQUIVALENT_NODE     =   NOT_EQUIVALENT,
    LESS_THAN_NODE          =   LESS_THAN,
    GREATER_THAN_NODE       =   GREATER_THAN,
    GREATER_OR_EQUAL_NODE   =   GREATER_OR_EQUAL,
    LESS_OR_EQUAL_NODE      =   LESS_OR_EQUAL,
    DOT_NODE                =   DOT,
    POWER_NODE              =   POWER,
    ROOT_NODE               =   ROOT,
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
        } conditional_block_node;

        struct {
            struct ASTNode* value;
        } return_block_node;
                   
        statement_list* list_node;

        // other node-specific data
    } data;

    bool reduced;

    bool block_flag;
    statement_list* block;

    specifiers node_specifiers;
};

struct statement_list{
    uint32_t max;
    uint32_t index;
    ASTNode** list;
};

#endif