#ifndef LIPL_AST_H
#define LIPL_AST_H

#include <stdbool.h>
#include <stdint.h>

#include "../../lexer/token_type.h"
#include "../../utility/data_pool.h"
#include "../../utility/stack.h"

typedef uint16_t specifiers;
#define SPEC_GLOBAL ((specifiers)0x0001u)
#define SPEC_CONST  ((specifiers)0x0002u)

typedef struct ASTNode ASTNode;
typedef struct statement_list statement_list;

typedef enum{
    NONE_AST_TYPE,
    LEAF_NODE,
    BINARY_OP_NODE,
    UNARY_OP_NODE,
    RES_WORD_NODE,
    FUNC_NODE,
    DECLARATION_NODE,
    CONDITIONAL_BLOCK_NODE,
}ASTNodeType;

typedef enum{
    ID_NODE,
    STR_NODE,
    INT_NODE,
    FLOAT_NODE,
    BOOL_NODE,
    NIL_NODE,
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
    AND_NODE                =   AND,
    OR_NODE                 =   OR,
    MODULO_NODE             =   MODULO,
}binary_op_node;

typedef enum{
    NEGATE_NODE,
    NOT_NODE,
} unary_op_node;

typedef enum{
    IF_NODE,
    ELSE_NODE,
    WHILE_NODE,
    FOR_NODE,
    ELIF_NODE,
} conditional_block_node;

typedef enum{
    RETURN_NODE,
    BREAK_NODE,
    CONTINUE_NODE,
} res_word_node;

typedef union{
    leaf_node leaf_node_value;
    binary_op_node binary_op_node_value;
    unary_op_node unary_op_node_value;
    conditional_block_node conditional_block_node_value;
    func_node func_node_value;
    res_word_node reserved_word_value;
} ASTNodeValue;

struct ASTNode{

    ASTNodeType type;
    ASTNodeValue value;     /* subtype; which member is valid depends on `type` */

    /* where the node starts in the source, for error messages in later passes */
    uint32_t line;
    uint32_t column;

    union {
        union{
            int      identifier;    /* ID_NODE: symbol table id */
            char*    string;        /* STR_NODE: owned by the ast_root */
            int64_t  integer;       /* INT_NODE */
            double   floating;      /* FLOAT_NODE */
            bool     boolean;       /* BOOL_NODE */
        } value_node;

        struct {
            struct ASTNode* lhs;
            struct ASTNode* rhs;
        } binary_op_node;

        struct {
            struct ASTNode* operand;
        } unary_op_node;

        /* call: arguments are expressions. declaration: arguments are ID leaves (parameters) */
        struct {
            int identifier;
            statement_list* arguments;
        } function_node;

        /* if / elif / while. else_branch links an if chain: IF -> ELIF -> ... -> ELSE */
        struct {
            struct ASTNode* condition;
            struct ASTNode* else_branch;
        } conditional_block_node;

        /* FOR_NODE; any part may be NULL */
        struct {
            struct ASTNode* initialiser;
            struct ASTNode* condition;
            struct ASTNode* step;
        } for_node;

        struct {
            struct ASTNode* value;  /* NULL for a bare `return;` (and always NULL for break/continue) */
        } return_block_node;

        struct {
            int identifier;
            struct ASTNode* initialiser;    /* NULL for `var x;` */
        } declaration_node;
    } data;

    /* body of a function declaration, if/elif/else, while or for */
    statement_list* block;

    specifiers node_specifiers;
};

struct statement_list{
    uint32_t count;
    uint32_t max;
    ASTNode** list;
};

/* A whole parsed program. Every node, list and string in the tree is owned
   here, so destroy_ast() frees it all without walking the tree. */
typedef struct ast_root{
    statement_list* statements;     /* top-level statements */
    data_pool*      node_pool;      /* every ASTNode */
    stack*          owned_lists;    /* every statement_list */
    stack*          owned_strings;  /* string literal contents */
} ast_root;

ast_root* create_ast_root(void);

/* Returns a zero-filled node of the given type. */
ASTNode* new_ast_node(ast_root* root, ASTNodeType type, uint32_t line, uint32_t column);

statement_list* new_ast_list(ast_root* root);

/* Copies `text` into storage owned by the tree. */
char* copy_ast_string(ast_root* root, const char* text);

void destroy_ast(ast_root* root);

#endif /* LIPL_AST_H */
