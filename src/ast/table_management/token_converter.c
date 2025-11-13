#include "token_converter.h"
#include "../ast_utility/routines.h"

#include <stdlib.h>
#include <stdio.h>

/* ---- specifier bits (apply to next declaration) ---------------------- */
#ifndef SPEC_GLOBAL_BIT
#define SPEC_GLOBAL_BIT ((specifiers)0x0001u)
#endif
#ifndef SPEC_CONST_BIT
#define SPEC_CONST_BIT  ((specifiers)0x0002u)
#endif

/* ---- small AST helpers ------------------------------------------------ */

static ASTNode* new_ast(table_iterator* it) {
    ASTNode* n = (ASTNode*)acquire_from_pool(it->node_pool);
    /* defensive init */
    safe_memset(n, 0, sizeof(*n));
    return n;
}

static void stage_node(table_iterator* it, ASTNode* n) {
    it->new_node_buffer          = n;
    it->new_node_buffer_set_flag = true;
}

/* ---- column mapping --------------------------------------------------- */

static uint32_t map_operator_col(operator_token op) {
    switch (op) {
        case ADDITION:
        case SUBTRACTION:     return COL_ADD_SUB;
        case MULTIPLICATION:
        case DIVISION:
        case POWER:
        case ROOT:            return COL_MUL_DIV;
        case ASSIGNMENT:      return COL_ASSIGN;
        case DOT:             return COL_DOT;
        case EQUIVALENT:
        case NOT_EQUIVALENT:
        case LESS_THAN:
        case GREATER_THAN:
        case GREATER_OR_EQUAL:
        case LESS_OR_EQUAL:   return COL_COMP;
        default:              return COL_EXPR; /* safe fallback */
    }
}

uint32_t token_to_column(const token* t, const token* next) {
    if (!t) return COL_EOS; /* treat missing as end of statement to unwind */

    switch (t->token_type) {
        case STRING_LITERAL:
        case INT_VALUE:
            return COL_EXPR;

        case IDENTIFIER: {
            /* Look-ahead: identifier + '(' -> function call head (COL_FUNC).
               else regular expression atom (COL_EXPR). */
            if (next && next->token_type == DELIMITER &&
                next->token_value.delimiter_token_value == OPEN_BRACKET) {
                return COL_FUNC;
            }
            return COL_EXPR;
        }

        case OPERATOR:
            return map_operator_col(t->token_value.operator_token_value);

        case DELIMITER: {
            switch (t->token_value.delimiter_token_value) {
                case EOS:            return COL_EOS;
                case COMMA:          return COL_COMMA;
                case OPEN_BRACKET:   return COL_OPEN_BRACKET;
                case CLOSE_BRACKET:  return COL_CLOSE_BRACKET;
                case OPEN_CBRACKET:  return COL_OPEN_CBRACKET;
                case CLOSE_CBRACKET: return COL_CLOSE_CBRACKET;
                default:             return COL_EXPR;
            }
        }

        case RESERVED_WORD: {
            switch (t->token_value.reserved_word_token_value) {
                case VAR:    return COL_VAR;
                case FUNC:   return COL_FDEC;      /* declaration keyword */
                case IF:
                case WHILE:
                case ELIF:   return COL_IF_WH_ELI;
                case ELSE:   return COL_ELSE;
                case RETURN: return COL_RETURN;
                case GLOBAL: return COL_GLOBAL;
                case CONST:  return COL_CONST;
                default:     return COL_EXPR;
            }
        }

        default:
            return COL_EXPR;
    }
}

/* ---- convert_token: stage AST node & return column ------------------- */

uint32_t convert_token(table_iterator* it, token** lookahead) {
    token* t   = (lookahead && *lookahead) ? *lookahead : NULL;
    token* nxt = t ? (token*)t->next : NULL;

    /* default: nothing staged */
    it->new_node_buffer_set_flag = false;
    it->new_node_buffer          = NULL;

    uint32_t col = token_to_column(t, nxt);

    if (!t) {
        return COL_EOS;
    }

    switch (t->token_type) {
        case STRING_LITERAL: {
            ASTNode* n = new_ast(it);
            n->type = LEAF_NODE;
            n->value.leaf_node_value = STR_NODE;
            /* borrow; tokens own payload */
            n->data.value_node.value = t->token_value.variable_value;
            stage_node(it, n);
            break;
        }

        case INT_VALUE: {
            ASTNode* n = new_ast(it);
            n->type = LEAF_NODE;
            n->value.leaf_node_value = INT_NODE;
            /* borrow; tokens own payload */
            n->data.value_node.value = t->token_value.variable_value;
            stage_node(it, n);
            break;
        }

        case IDENTIFIER: {
            if (col == COL_FUNC) {
                /* function call head: build FUNC_NODE carrying identifier id */
                ASTNode* n = new_ast(it);
                n->type = FUNC_NODE;
                n->value.func_node_value = FUNC_CALL_NODE; /* may be changed to DEC in reduction 5 */
                n->data.function_node.identifier       = t->token_value.identifier_token_value;
                n->data.function_node.arguments_count  = 0;
                n->data.function_node.arguments_list   = NULL;
                n->reduced    = false;
                n->block_flag = false;
                stage_node(it, n);
            } else {
                /* plain identifier as expression */
                ASTNode* n = new_ast(it);
                n->type = LEAF_NODE;
                n->value.leaf_node_value = ID_NODE;
                n->data.value_node.identifier = t->token_value.identifier_token_value;
                stage_node(it, n);
            }
            break;
        }

        case OPERATOR: {
            operator_token op = t->token_value.operator_token_value;
            ASTNode* n = new_ast(it);
            n->type = BINARY_OP_NODE;
            n->value.binary_op_node_value = (binary_op_node)op;
            n->data.binary_op_node.lhs = NULL;
            n->data.binary_op_node.rhs = NULL;
            stage_node(it, n);
            break;
        }

        case RESERVED_WORD: {
            switch (t->token_value.reserved_word_token_value) {
                case VAR:
                case FUNC:
                    /* Declaration keywords: no node, grammar uses COL_VAR/COL_FDEC */
                    break;

                case GLOBAL:
                    it->iterator_specifiers |= SPEC_GLOBAL_BIT;
                    break;

                case CONST:
                    it->iterator_specifiers |= SPEC_CONST_BIT;
                    break;

                case RETURN: {
                    ASTNode* n = new_ast(it);
                    n->type = RES_WORD_NODE;
                    n->value.reserved_word_value = RETURN_NODE;
                    stage_node(it, n);
                    break;
                }

                case IF:
                case WHILE:
                case ELIF: {
                    ASTNode* n = new_ast(it);
                    n->type = CONDITIONAL_BLOCK_NODE;
                    if (t->token_value.reserved_word_token_value == IF) {
                        n->value.conditional_block_node_value = IF_NODE;
                    } else if (t->token_value.reserved_word_token_value == WHILE) {
                        n->value.conditional_block_node_value = WHILE_NODE;
                    } else {
                        n->value.conditional_block_node_value = ELIF_NODE;
                    }
                    n->block_flag = false; /* will become true after rule 3 */
                    n->reduced    = false;
                    stage_node(it, n);
                    break;
                }

                case ELSE: {
                    /* else has no condition; it still produces a block node */
                    ASTNode* n = new_ast(it);
                    n->type = CONDITIONAL_BLOCK_NODE;
                    n->value.conditional_block_node_value = ELSE_NODE;
                    n->block_flag = true;  /* block follows immediately */
                    n->reduced    = true;  /* no further reduction needed before block */
                    stage_node(it, n);
                    break;
                }

                default:
                    /* ignore unknown reserved word here */
                    break;
            }
            break;
        }

        case DELIMITER:
            /* punctuation (comma, brackets, braces, semicolon): no node */
            break;

        default:
            /* Unrecognized; do not stage a node, keep as expression column to avoid OOB */
            fprintf(stderr, "convert_token: unexpected token type %d\n", t->token_type);
            return COL_EXPR;
    }

    return col;
}
