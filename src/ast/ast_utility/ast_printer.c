#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "ast_printer.h"
#include "../../lexer/lexer_main.h"

static void print_node(FILE* out, const ASTNode* node, int depth, const symbol_table* symbols);

static void indent(FILE* out, int depth){
    for (int i = 0; i < depth; ++i){
        fputs("  ", out);
    }
}

static void print_specifiers(FILE* out, specifiers flags){
    if (flags == 0) return;
    fputs(" [", out);
    if (flags & SPEC_GLOBAL){
        fputs("global", out);
    }
    if (flags & SPEC_CONST){
        fputs((flags & SPEC_GLOBAL) ? " const" : "const", out);
    }
    fputc(']', out);
}

static void print_list(FILE* out, const statement_list* list, int depth, const symbol_table* symbols){
    for (uint32_t i = 0; i < list->count; ++i){
        print_node(out, list->list[i], depth, symbols);
    }
}

static void print_block(FILE* out, const statement_list* block, int depth, const symbol_table* symbols){
    indent(out, depth);
    if (block->count == 0){
        fputs("{ }\n", out);
        return;
    }
    fputs("{\n", out);
    print_list(out, block, depth + 1, symbols);
    indent(out, depth);
    fputs("}\n", out);
}

/* "label:" followed by the node one level deeper, or "label: (none)" */
static void print_labelled(FILE* out, const char* label, const ASTNode* node, int depth, const symbol_table* symbols){
    indent(out, depth);
    if (node == NULL){
        fprintf(out, "%s (none)\n", label);
        return;
    }
    fprintf(out, "%s\n", label);
    print_node(out, node, depth + 1, symbols);
}

static void print_leaf(FILE* out, const ASTNode* node, const symbol_table* symbols){
    switch (node->value.leaf_node_value){
        case ID_NODE:
            fprintf(out, "ID %s\n", symbol_name(symbols, node->data.value_node.identifier));
            break;
        case INT_NODE:
            fprintf(out, "INT %" PRId64 "\n", node->data.value_node.integer);
            break;
        case FLOAT_NODE: {
            char number[48];
            format_float_literal(node->data.value_node.floating, number, sizeof number);
            fprintf(out, "FLOAT %s\n", number);
            break;
        }
        case BOOL_NODE:
            fprintf(out, "BOOL %s\n", node->data.value_node.boolean ? "true" : "false");
            break;
        case STR_NODE:
            fputs("STR ", out);
            print_string_literal(out, node->data.value_node.string);
            fputc('\n', out);
            break;
    }
}

static void print_function(FILE* out, const ASTNode* node, int depth, const symbol_table* symbols){
    const char* name = symbol_name(symbols, node->data.function_node.identifier);
    const statement_list* arguments = node->data.function_node.arguments;

    if (node->value.func_node_value == FUNC_CALL_NODE){
        if (arguments->count == 0){
            fprintf(out, "CALL %s (no arguments)\n", name);
        }else{
            fprintf(out, "CALL %s (%u argument%s)\n", name, (unsigned)arguments->count, arguments->count == 1 ? "" : "s");
        }
        print_list(out, arguments, depth + 1, symbols);
        return;
    }

    fprintf(out, "FUNC_DECL %s(", name);
    for (uint32_t i = 0; i < arguments->count; ++i){
        fprintf(out, "%s%s", i ? ", " : "", symbol_name(symbols, arguments->list[i]->data.value_node.identifier));
    }
    fputc(')', out);
    print_specifiers(out, node->node_specifiers);
    fputc('\n', out);
    print_block(out, node->block, depth + 1, symbols);
}

static void print_conditional(FILE* out, const ASTNode* node, int depth, const symbol_table* symbols){
    switch (node->value.conditional_block_node_value){
        case IF_NODE:
        case ELIF_NODE: {
            /* the whole if/elif/else chain prints at the same depth */
            fputs(node->value.conditional_block_node_value == IF_NODE ? "IF\n" : "ELIF\n", out);
            print_node(out, node->data.conditional_block_node.condition, depth + 1, symbols);
            print_block(out, node->block, depth + 1, symbols);
            const ASTNode* next = node->data.conditional_block_node.else_branch;
            if (next != NULL){
                print_node(out, next, depth, symbols);
            }
            break;
        }
        case ELSE_NODE:
            fputs("ELSE\n", out);
            print_block(out, node->block, depth + 1, symbols);
            break;
        case WHILE_NODE:
            fputs("WHILE\n", out);
            print_node(out, node->data.conditional_block_node.condition, depth + 1, symbols);
            print_block(out, node->block, depth + 1, symbols);
            break;
        case FOR_NODE:
            fputs("FOR\n", out);
            print_labelled(out, "init:", node->data.for_node.initialiser, depth + 1, symbols);
            print_labelled(out, "cond:", node->data.for_node.condition, depth + 1, symbols);
            print_labelled(out, "step:", node->data.for_node.step, depth + 1, symbols);
            print_block(out, node->block, depth + 1, symbols);
            break;
    }
}

static void print_node(FILE* out, const ASTNode* node, int depth, const symbol_table* symbols){
    indent(out, depth);

    switch (node->type){
        case LEAF_NODE:
            print_leaf(out, node, symbols);
            break;

        case BINARY_OP_NODE:
            fprintf(out, "BINOP %s\n", operator_lexeme((operator_token)node->value.binary_op_node_value));
            print_node(out, node->data.binary_op_node.lhs, depth + 1, symbols);
            print_node(out, node->data.binary_op_node.rhs, depth + 1, symbols);
            break;

        case UNARY_OP_NODE:
            fprintf(out, "UNARY %s\n", node->value.unary_op_node_value == NOT_NODE ? "!" : "-");
            print_node(out, node->data.unary_op_node.operand, depth + 1, symbols);
            break;

        case RES_WORD_NODE:
            fputs("RETURN\n", out);
            if (node->data.return_block_node.value != NULL){
                print_node(out, node->data.return_block_node.value, depth + 1, symbols);
            }
            break;

        case FUNC_NODE:
            print_function(out, node, depth, symbols);
            break;

        case DECLARATION_NODE:
            fprintf(out, "VAR_DECL %s", symbol_name(symbols, node->data.declaration_node.identifier));
            print_specifiers(out, node->node_specifiers);
            fputc('\n', out);
            if (node->data.declaration_node.initialiser != NULL){
                print_node(out, node->data.declaration_node.initialiser, depth + 1, symbols);
            }
            break;

        case CONDITIONAL_BLOCK_NODE:
            print_conditional(out, node, depth, symbols);
            break;

        case NONE_AST_TYPE:
            fputs("(empty node)\n", out);
            break;
    }
}

void print_ast(FILE* out, const ast_root* root, const symbol_table* symbols){
    print_list(out, root->statements, 0, symbols);
}
