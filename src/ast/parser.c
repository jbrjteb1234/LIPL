/*
 * Recursive-descent parser.
 *
 * Statements each have their own function. Expressions use precedence
 * climbing (Pratt parsing): parse_precedence(p, min) parses a prefix, then
 * keeps folding in infix operators that bind at least as tightly as `min`.
 *
 *   program     := statement* EOF
 *   statement   := declaration | if | while | for | return | expression ';'
 *   declaration := ('global' | 'const')* ( 'var' IDENT ('=' expression)? ';'
 *                                        | 'func' IDENT '(' params? ')' block )
 *   if          := 'if' expression block ('elif' expression block)* ('else' block)?
 *   while       := 'while' expression block
 *   for         := 'for' ['('] init? ';' expression? ';' expression? [')'] block
 *   return      := 'return' expression? ';'
 *   block       := '{' statement* '}'
 */
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "ast_utility/slist_functions.h"

/* Guards against blowing the C stack on input like ten thousand nested brackets */
#define MAX_NESTING_DEPTH 256u

typedef struct{
    token*              current;
    token*              previous;
    ast_root*           ast;
    const symbol_table* symbols;
    const char*         source_name;
    uint32_t            error_count;
    uint32_t            depth;
    bool                panic_mode;     /* suppresses follow-on errors until we resynchronise */
} parser;

/* Loosest to tightest */
typedef enum{
    PREC_NONE,
    PREC_ASSIGNMENT,    /* =           right-associative */
    PREC_OR,            /* ||                            */
    PREC_AND,           /* &&                            */
    PREC_EQUALITY,      /* == !=                         */
    PREC_COMPARISON,    /* < > <= >=                     */
    PREC_TERM,          /* + -                           */
    PREC_FACTOR,        /* * / %                         */
    PREC_UNARY,         /* prefix - !                    */
    PREC_POWER,         /* ^ _         right-associative */
    PREC_MEMBER,        /* .                             */
} precedence;

static ASTNode* parse_statement(parser* p);
static ASTNode* parse_expression(parser* p);
static ASTNode* parse_precedence(parser* p, precedence min_precedence);

/* ---- token helpers -------------------------------------------------------- */

static bool is_delimiter(const token* t, delimiter_token delimiter){
    return t->token_type == DELIMITER && t->token_value.delimiter_token_value == delimiter;
}

static bool is_operator(const token* t, operator_token op){
    return t->token_type == OPERATOR && t->token_value.operator_token_value == op;
}

static bool is_word(const token* t, reserved_word_token word){
    return t->token_type == RESERVED_WORD && t->token_value.reserved_word_token_value == word;
}

static bool at_end(const parser* p){
    return p->current->token_type == END_OF_INPUT;
}

static void advance(parser* p){
    p->previous = p->current;
    if (!at_end(p)){
        p->current = p->current->next;
    }
}

static bool match_delimiter(parser* p, delimiter_token delimiter){
    if (!is_delimiter(p->current, delimiter)) return false;
    advance(p);
    return true;
}

static bool match_word(parser* p, reserved_word_token word){
    if (!is_word(p->current, word)) return false;
    advance(p);
    return true;
}

static const char* name_of(const parser* p, const token* identifier){
    return symbol_name(p->symbols, identifier->token_value.identifier_token_value);
}

/* ---- errors --------------------------------------------------------------- */

static void error_at(parser* p, const token* t, const char* format, ...){
    if (p->panic_mode) return;
    p->panic_mode = true;
    p->error_count++;

    va_list args;
    fprintf(stderr, "%s:%u:%u: error: ", p->source_name, (unsigned)t->line, (unsigned)t->column);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
}

/* "expected <what>, found <current token>" */
static void error_expected(parser* p, const char* what){
    char found[96];
    describe_token(p->current, p->symbols, found, sizeof found);
    error_at(p, p->current, "expected %s, found %s", what, found);
}

static bool expect_delimiter(parser* p, delimiter_token delimiter, const char* what){
    if (match_delimiter(p, delimiter)) return true;
    error_expected(p, what);
    return false;
}

static bool starts_statement(const token* t){
    if (t->token_type != RESERVED_WORD) return false;
    switch (t->token_value.reserved_word_token_value){
        case VAR: case FUNC: case IF: case WHILE: case FOR:
        case RETURN: case GLOBAL: case CONST:
            return true;
        default:
            return false;
    }
}

/* After an error, skip to somewhere a new statement can start: just past a ';',
   just past a whole skipped {...} block, before a statement keyword, or before
   the '}' that closes the enclosing block. */
static void synchronize(parser* p){
    p->panic_mode = false;
    uint32_t depth = 0;

    while (!at_end(p)){
        token* skipped = p->current;
        if (depth == 0 && (is_delimiter(skipped, CLOSE_CBRACKET) || starts_statement(skipped))){
            return;
        }
        advance(p);
        if (is_delimiter(skipped, OPEN_CBRACKET)){
            depth++;
        }else if (is_delimiter(skipped, CLOSE_CBRACKET)){
            if (--depth == 0) return;
        }else if (depth == 0 && is_delimiter(skipped, EOS)){
            return;
        }
    }
}

static bool enter_nesting(parser* p){
    if (p->depth >= MAX_NESTING_DEPTH){
        error_at(p, p->current, "code is nested too deeply (the limit is %u levels)", MAX_NESTING_DEPTH);
        return false;
    }
    p->depth++;
    return true;
}

/* ---- node helpers --------------------------------------------------------- */

static ASTNode* make_node(parser* p, ASTNodeType type, const token* at){
    return new_ast_node(p->ast, type, at->line, at->column);
}

static ASTNode* make_identifier(parser* p, const token* identifier){
    ASTNode* node = make_node(p, LEAF_NODE, identifier);
    node->value.leaf_node_value = ID_NODE;
    node->data.value_node.identifier = identifier->token_value.identifier_token_value;
    return node;
}

/* ---- expressions ---------------------------------------------------------- */

static precedence infix_precedence(const token* t){
    if (t->token_type != OPERATOR) return PREC_NONE;
    switch (t->token_value.operator_token_value){
        case ASSIGNMENT:        return PREC_ASSIGNMENT;
        case OR:                return PREC_OR;
        case AND:               return PREC_AND;
        case EQUIVALENT:
        case NOT_EQUIVALENT:    return PREC_EQUALITY;
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_OR_EQUAL:
        case GREATER_OR_EQUAL:  return PREC_COMPARISON;
        case ADDITION:
        case SUBTRACTION:       return PREC_TERM;
        case MULTIPLICATION:
        case DIVISION:
        case MODULO:            return PREC_FACTOR;
        case POWER:
        case ROOT:              return PREC_POWER;
        case DOT:               return PREC_MEMBER;
        case NOT:               return PREC_NONE;   /* prefix only */
    }
    return PREC_NONE;
}

/* name(arg, arg, ...) - the current token is the '(' */
static ASTNode* parse_call(parser* p, const token* name){
    advance(p);

    ASTNode* call = make_node(p, FUNC_NODE, name);
    call->value.func_node_value = FUNC_CALL_NODE;
    call->data.function_node.identifier = name->token_value.identifier_token_value;
    call->data.function_node.arguments = new_ast_list(p->ast);

    if (!is_delimiter(p->current, CLOSE_BRACKET)){
        do {
            ASTNode* argument = parse_expression(p);
            if (!argument) return NULL;
            append_to_slist(call->data.function_node.arguments, argument);
        } while (match_delimiter(p, COMMA));
    }

    if (!expect_delimiter(p, CLOSE_BRACKET, "',' or ')' in argument list")) return NULL;
    return call;
}

static ASTNode* parse_prefix(parser* p){
    token* t = p->current;

    switch (t->token_type){
        case INT_VALUE: {
            advance(p);
            ASTNode* node = make_node(p, LEAF_NODE, t);
            node->value.leaf_node_value = INT_NODE;
            node->data.value_node.integer = t->token_value.int_value;
            return node;
        }
        case FLOAT_VALUE: {
            advance(p);
            ASTNode* node = make_node(p, LEAF_NODE, t);
            node->value.leaf_node_value = FLOAT_NODE;
            node->data.value_node.floating = t->token_value.float_value;
            return node;
        }
        case STRING_LITERAL: {
            advance(p);
            ASTNode* node = make_node(p, LEAF_NODE, t);
            node->value.leaf_node_value = STR_NODE;
            node->data.value_node.string = copy_ast_string(p->ast, t->token_value.string_value);
            return node;
        }
        case IDENTIFIER:
            advance(p);
            if (is_delimiter(p->current, OPEN_BRACKET)){
                return parse_call(p, t);
            }
            return make_identifier(p, t);

        case RESERVED_WORD:
            if (is_word(t, BOOL_TRUE) || is_word(t, BOOL_FALSE)){
                advance(p);
                ASTNode* node = make_node(p, LEAF_NODE, t);
                node->value.leaf_node_value = BOOL_NODE;
                node->data.value_node.boolean = is_word(t, BOOL_TRUE);
                return node;
            }
            break;

        case OPERATOR:
            if (is_operator(t, SUBTRACTION) || is_operator(t, NOT)){
                advance(p);
                ASTNode* operand = parse_precedence(p, PREC_UNARY);
                if (!operand) return NULL;
                ASTNode* node = make_node(p, UNARY_OP_NODE, t);
                node->value.unary_op_node_value = is_operator(t, NOT) ? NOT_NODE : NEGATE_NODE;
                node->data.unary_op_node.operand = operand;
                return node;
            }
            break;

        case DELIMITER:
            if (is_delimiter(t, OPEN_BRACKET)){
                advance(p);
                ASTNode* inner = parse_expression(p);
                if (!inner) return NULL;
                if (!is_delimiter(p->current, CLOSE_BRACKET)){
                    char found[96];
                    describe_token(p->current, p->symbols, found, sizeof found);
                    error_at(p, p->current, "expected ')' to close the '(' at %u:%u, found %s",
                             (unsigned)t->line, (unsigned)t->column, found);
                    return NULL;
                }
                advance(p);
                return inner;
            }
            break;

        default:
            break;
    }

    error_expected(p, "an expression");
    return NULL;
}

/* right-hand side of '.': a member name, or a method call */
static ASTNode* parse_member(parser* p){
    token* name = p->current;
    if (name->token_type != IDENTIFIER){
        error_expected(p, "a member name after '.'");
        return NULL;
    }
    advance(p);
    if (is_delimiter(p->current, OPEN_BRACKET)){
        return parse_call(p, name);
    }
    return make_identifier(p, name);
}

static bool is_assignment_target(const ASTNode* node){
    if (node->type == LEAF_NODE && node->value.leaf_node_value == ID_NODE) return true;
    /* a.b = ... but not a.f() = ... */
    if (node->type == BINARY_OP_NODE && node->value.binary_op_node_value == DOT_NODE){
        const ASTNode* member = node->data.binary_op_node.rhs;
        return member->type == LEAF_NODE && member->value.leaf_node_value == ID_NODE;
    }
    return false;
}

static ASTNode* parse_precedence_inner(parser* p, precedence min_precedence){
    ASTNode* left = parse_prefix(p);
    if (!left) return NULL;

    for (;;){
        token* op = p->current;
        precedence op_precedence = infix_precedence(op);
        if (op_precedence == PREC_NONE || op_precedence < min_precedence){
            break;
        }
        advance(p);

        ASTNode* right;
        if (is_operator(op, DOT)){
            right = parse_member(p);
        }else if (is_operator(op, ASSIGNMENT)){
            if (!is_assignment_target(left)){
                error_at(p, op, "can only assign to a variable or a member (x = ..., a.b = ...)");
                return NULL;
            }
            right = parse_precedence(p, PREC_ASSIGNMENT);
        }else if (op_precedence == PREC_POWER){
            right = parse_precedence(p, PREC_POWER);
        }else{
            right = parse_precedence(p, (precedence)(op_precedence + 1));
        }
        if (!right) return NULL;

        ASTNode* node = make_node(p, BINARY_OP_NODE, op);
        node->value.binary_op_node_value = (binary_op_node)op->token_value.operator_token_value;
        node->data.binary_op_node.lhs = left;
        node->data.binary_op_node.rhs = right;
        left = node;
    }
    return left;
}

static ASTNode* parse_precedence(parser* p, precedence min_precedence){
    if (!enter_nesting(p)) return NULL;
    ASTNode* result = parse_precedence_inner(p, min_precedence);
    p->depth--;
    return result;
}

static ASTNode* parse_expression(parser* p){
    return parse_precedence(p, PREC_ASSIGNMENT);
}

/* ---- statements ----------------------------------------------------------- */

/* Statements up to the end of the file, or up to a '}' when inside a block */
static void parse_statements(parser* p, statement_list* list, bool inside_block){
    while (!at_end(p) && !(inside_block && is_delimiter(p->current, CLOSE_CBRACKET))){
        token* before = p->current;

        ASTNode* statement = parse_statement(p);
        if (statement){
            append_to_slist(list, statement);
        }else{
            synchronize(p);
        }

        /* always make progress, even if the bad token was where synchronize stops */
        if (p->current == before){
            advance(p);
        }
    }
}

/* '{' statement* '}' - `owner` names what the block belongs to, for error messages */
static statement_list* parse_block(parser* p, const char* owner){
    token* open = p->current;
    if (!is_delimiter(open, OPEN_CBRACKET)){
        char what[80];
        snprintf(what, sizeof what, "'{' to start the body of %s", owner);
        error_expected(p, what);
        return NULL;
    }
    if (!enter_nesting(p)) return NULL;
    advance(p);

    statement_list* block = new_ast_list(p->ast);
    parse_statements(p, block, true);
    p->depth--;

    if (!is_delimiter(p->current, CLOSE_CBRACKET)){
        char found[96];
        describe_token(p->current, p->symbols, found, sizeof found);
        error_at(p, p->current, "expected '}' to close the block opened at %u:%u, found %s",
                 (unsigned)open->line, (unsigned)open->column, found);
        return NULL;
    }
    advance(p);
    return block;
}

/* 'var' has been consumed */
static ASTNode* parse_var_declaration(parser* p, specifiers flags, const token* keyword){
    token* name = p->current;
    if (name->token_type != IDENTIFIER){
        error_expected(p, "a variable name after 'var'");
        return NULL;
    }
    advance(p);

    ASTNode* declaration = make_node(p, DECLARATION_NODE, keyword);
    declaration->data.declaration_node.identifier = name->token_value.identifier_token_value;
    declaration->node_specifiers = flags;

    if (is_operator(p->current, ASSIGNMENT)){
        advance(p);
        declaration->data.declaration_node.initialiser = parse_expression(p);
        if (!declaration->data.declaration_node.initialiser) return NULL;
    }else if (flags & SPEC_CONST){
        error_at(p, name, "const variable '%s' needs an initial value", name_of(p, name));
        return NULL;
    }

    if (!expect_delimiter(p, EOS, "';' after variable declaration")) return NULL;
    return declaration;
}

/* 'func' has been consumed */
static ASTNode* parse_function_declaration(parser* p, specifiers flags, const token* keyword){
    token* name = p->current;
    if (name->token_type != IDENTIFIER){
        error_expected(p, "a function name after 'func'");
        return NULL;
    }
    advance(p);
    if (!expect_delimiter(p, OPEN_BRACKET, "'(' after the function name")) return NULL;

    ASTNode* function = make_node(p, FUNC_NODE, keyword);
    function->value.func_node_value = FUNC_DEC_NODE;
    function->data.function_node.identifier = name->token_value.identifier_token_value;
    function->data.function_node.arguments = new_ast_list(p->ast);
    function->node_specifiers = flags;
    statement_list* parameters = function->data.function_node.arguments;

    if (!is_delimiter(p->current, CLOSE_BRACKET)){
        do {
            token* parameter = p->current;
            if (parameter->token_type != IDENTIFIER){
                error_expected(p, "a parameter name");
                return NULL;
            }
            for (uint32_t i = 0; i < parameters->count; ++i){
                if (parameters->list[i]->data.value_node.identifier == parameter->token_value.identifier_token_value){
                    error_at(p, parameter, "duplicate parameter '%s'", name_of(p, parameter));
                    return NULL;
                }
            }
            advance(p);
            append_to_slist(parameters, make_identifier(p, parameter));
        } while (match_delimiter(p, COMMA));
    }
    if (!expect_delimiter(p, CLOSE_BRACKET, "',' or ')' in parameter list")) return NULL;

    char owner[80];
    snprintf(owner, sizeof owner, "function '%s'", name_of(p, name));
    function->block = parse_block(p, owner);
    if (!function->block) return NULL;
    return function;
}

static ASTNode* parse_declaration(parser* p){
    specifiers flags = 0;
    while (is_word(p->current, GLOBAL) || is_word(p->current, CONST)){
        specifiers flag = is_word(p->current, GLOBAL) ? SPEC_GLOBAL : SPEC_CONST;
        if (flags & flag){
            error_at(p, p->current, "'%s' is repeated",
                     fixed_lexeme(p->current->token_type, p->current->token_value));
            return NULL;
        }
        flags = (specifiers)(flags | flag);
        advance(p);
    }

    if (match_word(p, VAR)) return parse_var_declaration(p, flags, p->previous);
    if (match_word(p, FUNC)) return parse_function_declaration(p, flags, p->previous);

    error_expected(p, "'var' or 'func' after 'global'/'const'");
    return NULL;
}

/* condition and block of an if / elif / while; the keyword has been consumed */
static ASTNode* parse_conditional_branch(parser* p, const token* keyword, conditional_block_node kind){
    ASTNode* node = make_node(p, CONDITIONAL_BLOCK_NODE, keyword);
    node->value.conditional_block_node_value = kind;

    node->data.conditional_block_node.condition = parse_expression(p);
    if (!node->data.conditional_block_node.condition) return NULL;

    char owner[16];
    snprintf(owner, sizeof owner, "'%s'", fixed_lexeme(keyword->token_type, keyword->token_value));
    node->block = parse_block(p, owner);
    if (!node->block) return NULL;
    return node;
}

/* if ... elif ... else ..., linked through else_branch */
static ASTNode* parse_if(parser* p){
    advance(p);
    ASTNode* head = parse_conditional_branch(p, p->previous, IF_NODE);
    if (!head) return NULL;

    ASTNode* tail = head;
    while (match_word(p, ELIF)){
        ASTNode* branch = parse_conditional_branch(p, p->previous, ELIF_NODE);
        if (!branch) return NULL;
        tail->data.conditional_block_node.else_branch = branch;
        tail = branch;
    }

    if (match_word(p, ELSE)){
        token* keyword = p->previous;
        if (is_word(p->current, IF)){
            error_at(p, p->current, "write 'elif' instead of 'else if'");
            return NULL;
        }
        ASTNode* branch = make_node(p, CONDITIONAL_BLOCK_NODE, keyword);
        branch->value.conditional_block_node_value = ELSE_NODE;
        branch->block = parse_block(p, "'else'");
        if (!branch->block) return NULL;
        tail->data.conditional_block_node.else_branch = branch;
    }
    return head;
}

static ASTNode* parse_while(parser* p){
    advance(p);
    return parse_conditional_branch(p, p->previous, WHILE_NODE);
}

/* for init; condition; step { ... } - brackets around the header are optional */
static ASTNode* parse_for(parser* p){
    token* keyword = p->current;
    advance(p);
    bool bracketed = match_delimiter(p, OPEN_BRACKET);

    ASTNode* loop = make_node(p, CONDITIONAL_BLOCK_NODE, keyword);
    loop->value.conditional_block_node_value = FOR_NODE;

    /* initialiser: a var declaration (which eats its own ';'), an expression, or nothing */
    if (match_word(p, VAR)){
        loop->data.for_node.initialiser = parse_var_declaration(p, 0, p->previous);
        if (!loop->data.for_node.initialiser) return NULL;
    }else if (!match_delimiter(p, EOS)){
        loop->data.for_node.initialiser = parse_expression(p);
        if (!loop->data.for_node.initialiser) return NULL;
        if (!expect_delimiter(p, EOS, "';' after the for-loop initialiser")) return NULL;
    }

    if (!match_delimiter(p, EOS)){
        loop->data.for_node.condition = parse_expression(p);
        if (!loop->data.for_node.condition) return NULL;
        if (!expect_delimiter(p, EOS, "';' after the for-loop condition")) return NULL;
    }

    if (!is_delimiter(p->current, bracketed ? CLOSE_BRACKET : OPEN_CBRACKET)){
        loop->data.for_node.step = parse_expression(p);
        if (!loop->data.for_node.step) return NULL;
    }
    if (bracketed && !expect_delimiter(p, CLOSE_BRACKET, "')' to close the for-loop header")) return NULL;

    loop->block = parse_block(p, "'for'");
    if (!loop->block) return NULL;
    return loop;
}

static ASTNode* parse_return(parser* p){
    token* keyword = p->current;
    advance(p);

    ASTNode* node = make_node(p, RES_WORD_NODE, keyword);
    node->value.reserved_word_value = RETURN_NODE;

    if (!match_delimiter(p, EOS)){
        node->data.return_block_node.value = parse_expression(p);
        if (!node->data.return_block_node.value) return NULL;
        if (!expect_delimiter(p, EOS, "';' after the return value")) return NULL;
    }
    return node;
}

static ASTNode* parse_statement(parser* p){
    token* t = p->current;

    if (t->token_type == RESERVED_WORD){
        switch (t->token_value.reserved_word_token_value){
            case GLOBAL:
            case CONST:
            case VAR:
            case FUNC:      return parse_declaration(p);
            case IF:        return parse_if(p);
            case WHILE:     return parse_while(p);
            case FOR:       return parse_for(p);
            case RETURN:    return parse_return(p);
            case ELIF:
            case ELSE:
                error_at(p, t, "'%s' without a matching 'if'", fixed_lexeme(t->token_type, t->token_value));
                return NULL;
            case BOOL_TRUE:
            case BOOL_FALSE:
                break;      /* an expression statement */
        }
    }
    if (is_delimiter(t, OPEN_CBRACKET)){
        error_at(p, t, "unexpected '{': a block can only follow if, elif, else, while, for or func");
        return NULL;
    }
    if (is_delimiter(t, CLOSE_CBRACKET)){
        error_at(p, t, "unexpected '}' with no block to close");
        return NULL;
    }

    ASTNode* expression = parse_expression(p);
    if (!expression) return NULL;
    if (!expect_delimiter(p, EOS, "';' after expression")) return NULL;
    return expression;
}

/* ---- entry point ---------------------------------------------------------- */

ast_root* parse(const token_stream* tokens, const symbol_table* symbols){
    parser p = {
        .current = tokens->head,
        .previous = NULL,
        .ast = create_ast_root(),
        .symbols = symbols,
        .source_name = tokens->source_name,
        .error_count = 0,
        .depth = 0,
        .panic_mode = false,
    };

    parse_statements(&p, p.ast->statements, false);

    if (p.error_count > 0){
        destroy_ast(p.ast);
        return NULL;
    }
    return p.ast;
}
