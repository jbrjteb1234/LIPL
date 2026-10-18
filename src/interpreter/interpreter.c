/*
 * Tree-walking interpreter.
 *
 * eval() turns an expression node into a value; exec_statement() runs a
 * statement and reports whether it finished normally or hit return, break or
 * continue. Runtime errors print a message and longjmp back to interpret(),
 * which frees everything that's left.
 *
 * Language rules decided here:
 *   - only false and nil are false
 *   - int / int truncates like C (7 / 2 == 3); any float makes the result a float
 *   - a _ b is the b-th root of a (27 _ 3 == 3)
 *   - a.f(x) is f(a, x), so list.push(4) and line.split(",") work
 *   - `global var x` inside a function creates x at the top level
 *   - top-level variables can be redeclared (unless const); locals can't
 */
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "runtime.h"
#include "../lexer/lexer_main.h"
#include "../utility/diagnostics.h"
#include "../utility/platform.h"
#include "../utility/safe_memory.h"

/* Recursion may use this fraction of the real C stack before it's reported as an
   error. The rest is headroom for builtins and printing between checks. */
#define STACK_USE_PERCENT 75u

/* if the stack size can't be found out (or is unlimited) */
#define FALLBACK_STACK_LIMIT ((uintptr_t)1024u * 1024u)

typedef enum{
    EXEC_NORMAL,
    EXEC_RETURN,
    EXEC_BREAK,
    EXEC_CONTINUE,
} exec_status;

static value eval(interpreter* interp, const ASTNode* node);
static exec_status exec_statements(interpreter* interp, const statement_list* statements);
static exec_status exec_block(interpreter* interp, const statement_list* block);

/* ---- errors --------------------------------------------------------------- */

_Noreturn void runtime_error(interpreter* interp, const ASTNode* at, const char* format, ...){
    fflush(stdout);     /* so output printed before the error appears before it */
    va_list args;
    va_start(args, format);
    report_error(interp->source, at->line, at->column, format, args);
    va_end(args);
    longjmp(interp->error_jump, 1);
}

/* Where the C stack is now. This uses the frame address rather than the address
   of a local variable, because AddressSanitizer's use-after-return checking (on
   by default in newer compilers) moves locals onto a separate heap "fake stack". */
static uintptr_t stack_position(void){
    return (uintptr_t)__builtin_frame_address(0);
}

static void check_stack(interpreter* interp, const ASTNode* at){
    uintptr_t here = stack_position();
    uintptr_t used = interp->stack_base > here ? interp->stack_base - here : here - interp->stack_base;
    if (used > interp->stack_limit){
        runtime_error(interp, at, "stack overflow: too much recursion");
    }
}

static const char* name_of(const interpreter* interp, int identifier){
    return symbol_name(interp->symbols, identifier);
}

/* ---- variables ------------------------------------------------------------ */

static binding* find_variable(interpreter* interp, int identifier){
    for (scope* variables = interp->current; variables != NULL; variables = variables->parent){
        for (uint32_t i = 0; i < variables->count; ++i){
            if (variables->bindings[i].identifier == identifier){
                return &variables->bindings[i];
            }
        }
    }
    binding* global = &interp->globals[identifier];
    return global->identifier == identifier ? global : NULL;
}

/* the variable's value (owned), or the builtin of that name */
static value lookup(interpreter* interp, int identifier, const ASTNode* at){
    binding* variable = find_variable(interp, identifier);
    if (variable) return retain(variable->contents);
    const builtin* native = interp->builtins[identifier];
    if (native) return (value){ .type = VAL_BUILTIN, .as.native = native };
    runtime_error(interp, at, "'%s' is not defined", name_of(interp, identifier));
}

/* takes ownership of contents */
static void declare(interpreter* interp, int identifier, value contents, specifiers flags, const ASTNode* at){
    bool is_const = (flags & SPEC_CONST) != 0;

    if ((flags & SPEC_GLOBAL) || interp->current == NULL){
        binding* global = &interp->globals[identifier];
        if (global->identifier == identifier){
            if (global->is_const){
                runtime_error(interp, at, "'%s' is a constant and can't be redeclared", name_of(interp, identifier));
            }
            value old = global->contents;
            global->contents = contents;
            global->is_const = is_const;
            release(interp, old);
            return;
        }
        *global = (binding){ .identifier = identifier, .is_const = is_const, .contents = contents };
        return;
    }

    scope* variables = interp->current;
    for (uint32_t i = 0; i < variables->count; ++i){
        if (variables->bindings[i].identifier == identifier){
            runtime_error(interp, at, "'%s' is already declared in this scope", name_of(interp, identifier));
        }
    }
    if (variables->count == variables->capacity){
        variables->capacity *= 2u;
        variables->bindings = (binding*)safe_realloc(variables->bindings, sizeof(binding) * variables->capacity);
    }
    variables->bindings[variables->count++] = (binding){ .identifier = identifier, .is_const = is_const, .contents = contents };
}

/* takes ownership of contents */
static void assign_variable(interpreter* interp, int identifier, value contents, const ASTNode* at){
    binding* variable = find_variable(interp, identifier);
    if (variable == NULL){
        const char* name = name_of(interp, identifier);
        runtime_error(interp, at, "'%s' isn't declared yet: use 'var %s = ...' to create it", name, name);
    }
    if (variable->is_const){
        runtime_error(interp, at, "'%s' is a constant and can't be changed", name_of(interp, identifier));
    }
    value old = variable->contents;
    variable->contents = contents;
    release(interp, old);
}

/* ---- arithmetic ----------------------------------------------------------- */

static bool is_number(value v){
    return v.type == VAL_INT || v.type == VAL_FLOAT;
}

static double as_double(value v){
    return v.type == VAL_INT ? (double)v.as.integer : v.as.floating;
}

static _Noreturn void operand_error(interpreter* interp, const ASTNode* at, binary_op_node op, value a, value b){
    runtime_error(interp, at, "can't use '%s' on %s and %s",
                  operator_lexeme((operator_token)op), type_name(a), type_name(b));
}

static _Noreturn void overflow_error(interpreter* interp, const ASTNode* at){
    runtime_error(interp, at, "integer overflow: the result doesn't fit in 64 bits");
}

/* NaN never becomes a LIPL value, so comparisons always mean something */
static value float_result(interpreter* interp, double result, const ASTNode* at){
    if (__builtin_isnan(result)){
        runtime_error(interp, at, "the result isn't a number");
    }
    return float_value(result);
}

static bool checked_power(int64_t base, int64_t exponent, int64_t* out){
    int64_t result = 1;
    while (exponent > 0){
        if ((exponent & 1) && __builtin_mul_overflow(result, base, &result)) return false;
        exponent >>= 1;
        if (exponent > 0 && __builtin_mul_overflow(base, base, &base)) return false;
    }
    *out = result;
    return true;
}

/* a _ b: the b-th root of a. Whole-number answers to whole-number questions stay ints. */
static value root(interpreter* interp, value a, value b, const ASTNode* at){
    double radicand = as_double(a);
    double degree = as_double(b);
    if (degree == 0.0){
        runtime_error(interp, at, "can't take a 0th root");
    }
    bool odd_whole_degree = b.type == VAL_INT && b.as.integer % 2 != 0;
    if (radicand < 0.0 && !odd_whole_degree){
        runtime_error(interp, at, "a negative number only has odd whole-number roots");
    }

    double magnitude = fabs(radicand);
    double result = degree == 2.0 ? sqrt(magnitude)
                  : degree == 3.0 ? cbrt(magnitude)
                  : pow(magnitude, 1.0 / degree);
    if (radicand < 0.0) result = -result;

    if (a.type == VAL_INT && b.type == VAL_INT && b.as.integer > 0 && fabs(result) < 9.0e18){
        int64_t whole = (int64_t)llround(result);
        int64_t check;
        if (checked_power(whole, b.as.integer, &check) && check == a.as.integer){
            return int_value(whole);
        }
    }
    return float_result(interp, result, at);
}

static int compare(interpreter* interp, binary_op_node op, value a, value b, const ASTNode* at){
    if (a.type == VAL_INT && b.type == VAL_INT){
        return (a.as.integer > b.as.integer) - (a.as.integer < b.as.integer);
    }
    if (is_number(a) && is_number(b)){
        double x = as_double(a);
        double y = as_double(b);
        return (x > y) - (x < y);
    }
    if (a.type == VAL_STRING && b.type == VAL_STRING){
        obj_string* x = AS_STRING(a);
        obj_string* y = AS_STRING(b);
        size_t shorter = x->length < y->length ? x->length : y->length;
        int order = memcmp(x->chars, y->chars, shorter);
        if (order != 0) return order < 0 ? -1 : 1;
        return (x->length > y->length) - (x->length < y->length);
    }
    operand_error(interp, at, op, a, b);
}

/* "a" + anything, or anything + "a": joins their printed forms */
static value concatenate(interpreter* interp, value a, value b, const ASTNode* at){
    string_builder text;
    sb_init(&text);
    format_value(interp, &text, a, false);
    format_value(interp, &text, b, false);
    if (text.length > MAX_STRING_LENGTH){
        sb_free(&text);
        runtime_error(interp, at, "string is too long");
    }
    return take_string(interp, &text);
}

static value repeat_string(interpreter* interp, obj_string* text, int64_t times, const ASTNode* at){
    if (times < 0){
        runtime_error(interp, at, "can't repeat a string a negative number of times");
    }
    if (text->length > 0 && (uint64_t)times > MAX_STRING_LENGTH / text->length){
        runtime_error(interp, at, "string is too long");
    }
    obj_string* result = allocate_string(interp, text->length * (size_t)times);
    for (int64_t i = 0; i < times; ++i){
        memcpy(result->chars + (size_t)i * text->length, text->chars, text->length);
    }
    return object_value(VAL_STRING, &result->header);
}

static value join_lists(interpreter* interp, obj_list* first, obj_list* second, const ASTNode* at){
    obj_list* joined = new_list(interp, first->count + second->count);
    for (uint32_t i = 0; i < first->count; ++i) list_push(interp, joined, retain(first->items[i]), at);
    for (uint32_t i = 0; i < second->count; ++i) list_push(interp, joined, retain(second->items[i]), at);
    return object_value(VAL_LIST, &joined->header);
}

static value binary_operation(interpreter* interp, binary_op_node op, value a, value b, const ASTNode* at){
    bool both_ints = a.type == VAL_INT && b.type == VAL_INT;
    bool both_numbers = is_number(a) && is_number(b);
    int64_t result;

    switch (op){
        case ADDITION_NODE:
            if (a.type == VAL_STRING || b.type == VAL_STRING) return concatenate(interp, a, b, at);
            if (a.type == VAL_LIST && b.type == VAL_LIST) return join_lists(interp, AS_LIST(a), AS_LIST(b), at);
            if (both_ints){
                if (__builtin_add_overflow(a.as.integer, b.as.integer, &result)) overflow_error(interp, at);
                return int_value(result);
            }
            if (both_numbers) return float_result(interp, as_double(a) + as_double(b), at);
            break;

        case SUBTRACTION_NODE:
            if (both_ints){
                if (__builtin_sub_overflow(a.as.integer, b.as.integer, &result)) overflow_error(interp, at);
                return int_value(result);
            }
            if (both_numbers) return float_result(interp, as_double(a) - as_double(b), at);
            break;

        case MULTIPLICATION_NODE:
            if (a.type == VAL_STRING && b.type == VAL_INT) return repeat_string(interp, AS_STRING(a), b.as.integer, at);
            if (a.type == VAL_INT && b.type == VAL_STRING) return repeat_string(interp, AS_STRING(b), a.as.integer, at);
            if (both_ints){
                if (__builtin_mul_overflow(a.as.integer, b.as.integer, &result)) overflow_error(interp, at);
                return int_value(result);
            }
            if (both_numbers) return float_result(interp, as_double(a) * as_double(b), at);
            break;

        case DIVISION_NODE:
            if (!both_numbers) break;
            if (as_double(b) == 0.0) runtime_error(interp, at, "division by zero");
            if (both_ints){
                if (a.as.integer == INT64_MIN && b.as.integer == -1) overflow_error(interp, at);
                return int_value(a.as.integer / b.as.integer);
            }
            return float_result(interp, as_double(a) / as_double(b), at);

        case MODULO_NODE:
            if (!both_numbers) break;
            if (as_double(b) == 0.0) runtime_error(interp, at, "modulo by zero");
            if (both_ints){
                if (b.as.integer == -1) return int_value(0);     /* INT64_MIN % -1 would trap */
                return int_value(a.as.integer % b.as.integer);
            }
            return float_result(interp, fmod(as_double(a), as_double(b)), at);

        case POWER_NODE:
            if (!both_numbers) break;
            if (both_ints && b.as.integer >= 0){
                if (!checked_power(a.as.integer, b.as.integer, &result)) overflow_error(interp, at);
                return int_value(result);
            }
            return float_result(interp, pow(as_double(a), as_double(b)), at);

        case ROOT_NODE:
            if (!both_numbers) break;
            return root(interp, a, b, at);

        case EQUIVALENT_NODE:       return bool_value(values_equal(interp, a, b, at));
        case NOT_EQUIVALENT_NODE:   return bool_value(!values_equal(interp, a, b, at));
        case LESS_THAN_NODE:        return bool_value(compare(interp, op, a, b, at) < 0);
        case GREATER_THAN_NODE:     return bool_value(compare(interp, op, a, b, at) > 0);
        case LESS_OR_EQUAL_NODE:    return bool_value(compare(interp, op, a, b, at) <= 0);
        case GREATER_OR_EQUAL_NODE: return bool_value(compare(interp, op, a, b, at) >= 0);

        case ASSIGNMENT_NODE:
        case DOT_NODE:
        case AND_NODE:
        case OR_NODE:
            break;  /* evaluated before their operands are, in eval_binary */
    }
    operand_error(interp, at, op, a, b);
}

/* ---- calls ---------------------------------------------------------------- */

static _Noreturn void arity_error(interpreter* interp, const ASTNode* at, const char* name,
                                  uint32_t min_args, uint32_t max_args, uint32_t given){
    if (min_args == max_args){
        runtime_error(interp, at, "%s() takes %u argument%s but was given %u",
                      name, (unsigned)min_args, min_args == 1 ? "" : "s", (unsigned)given);
    }
    if (max_args == UINT32_MAX){
        runtime_error(interp, at, "%s() takes at least %u argument%s but was given %u",
                      name, (unsigned)min_args, min_args == 1 ? "" : "s", (unsigned)given);
    }
    runtime_error(interp, at, "%s() takes %u to %u arguments but was given %u",
                  name, (unsigned)min_args, (unsigned)max_args, (unsigned)given);
}

/* args stay owned by the caller. They point into the argument stack, so they're
   only read before the function body runs (which may grow and move that stack). */
static value call_value(interpreter* interp, value callee, const value* args, uint32_t count, const ASTNode* call){
    if (callee.type == VAL_BUILTIN){
        const builtin* native = callee.as.native;
        if (count < native->min_args || count > native->max_args){
            arity_error(interp, call, native->name, native->min_args, native->max_args, count);
        }
        return native->fn(interp, args, count, call);
    }
    if (callee.type != VAL_FUNCTION){
        const char* article = callee.type == VAL_NIL ? "" : callee.type == VAL_INT ? "an " : "a ";
        runtime_error(interp, call, "can't call '%s': it's %s%s, not a function",
                      name_of(interp, call->data.function_node.identifier), article, type_name(callee));
    }

    obj_function* function = AS_FUNCTION(callee);
    const ASTNode* declaration = function->declaration;
    const statement_list* parameters = declaration->data.function_node.arguments;
    if (count != parameters->count){
        arity_error(interp, call, name_of(interp, declaration->data.function_node.identifier),
                    parameters->count, parameters->count, count);
    }
    check_stack(interp, call);

    scope* frame = new_scope(interp, function->closure);
    scope* caller = interp->current;
    interp->current = frame;
    for (uint32_t i = 0; i < count; ++i){
        declare(interp, parameters->list[i]->data.value_node.identifier, retain(args[i]), 0, call);
    }

    interp->call_depth++;
    exec_status status = exec_statements(interp, declaration->block);
    interp->call_depth--;
    interp->current = caller;
    release_object(interp, &frame->header);

    if (status == EXEC_RETURN){
        value result = interp->return_value;
        interp->return_value = nil_value();
        return result;
    }
    return nil_value();
}

static void push_argument(interpreter* interp, value argument, const ASTNode* call){
    if (interp->argument_count == interp->argument_capacity){
        if (interp->argument_capacity > UINT32_MAX / 2u){
            runtime_error(interp, call, "too many arguments");
        }
        interp->argument_capacity = interp->argument_capacity ? interp->argument_capacity * 2u : 64u;
        interp->arguments = (value*)safe_realloc(interp->arguments, sizeof(value) * interp->argument_capacity);
    }
    interp->arguments[interp->argument_count++] = argument;
}

/* name(args), or receiver.name(args) which is name(receiver, args). Takes ownership of *receiver. */
static value eval_call(interpreter* interp, const ASTNode* call, const value* receiver){
    const statement_list* arguments = call->data.function_node.arguments;

    /* owned, so reassigning the name while the arguments run can't free the function */
    value callee = lookup(interp, call->data.function_node.identifier, call);

    /* nested calls made while evaluating the arguments push and pop above `base` */
    uint32_t base = interp->argument_count;
    if (receiver) push_argument(interp, *receiver, call);
    for (uint32_t i = 0; i < arguments->count; ++i){
        push_argument(interp, eval(interp, arguments->list[i]), call);
    }
    uint32_t count = interp->argument_count - base;

    value result = call_value(interp, callee, interp->arguments + base, count, call);

    /* the array may have moved during the call, so index it afresh */
    for (uint32_t i = base; i < base + count; ++i){
        release(interp, interp->arguments[i]);
    }
    interp->argument_count = base;
    release(interp, callee);
    return result;
}

/* ---- expressions ---------------------------------------------------------- */

static value eval_leaf(interpreter* interp, const ASTNode* node){
    switch (node->value.leaf_node_value){
        case ID_NODE:       return lookup(interp, node->data.value_node.identifier, node);
        case INT_NODE:      return int_value(node->data.value_node.integer);
        case FLOAT_NODE:    return float_value(node->data.value_node.floating);
        case BOOL_NODE:     return bool_value(node->data.value_node.boolean);
        case NIL_NODE:      return nil_value();
        case STR_NODE: {
            const char* text = node->data.value_node.string;
            return make_string(interp, text, strlen(text));
        }
    }
    runtime_error(interp, node, "internal error: unknown literal");
}

/* list[i] or string[i]; negative i counts from the end */
static uint32_t resolve_index(interpreter* interp, value index, size_t length, const ASTNode* at){
    if (index.type != VAL_INT){
        runtime_error(interp, at, "indexes must be ints, not %s", type_name(index));
    }
    int64_t position = index.as.integer;
    if (position < 0) position += (int64_t)length;
    if (position < 0 || (uint64_t)position >= length){
        runtime_error(interp, at, "index %" PRId64 " is out of range for length %" PRIu64,
                      index.as.integer, (uint64_t)length);
    }
    return (uint32_t)position;
}

static value eval_index(interpreter* interp, const ASTNode* node){
    value target = eval(interp, node->data.index_node.target);
    value index = eval(interp, node->data.index_node.index);
    value result;

    if (target.type == VAL_LIST){
        obj_list* list = AS_LIST(target);
        result = retain(list->items[resolve_index(interp, index, list->count, node)]);
    }else if (target.type == VAL_STRING){
        obj_string* text = AS_STRING(target);
        uint32_t position = resolve_index(interp, index, text->length, node);
        result = make_string(interp, text->chars + position, 1);
    }else{
        runtime_error(interp, node, "can't index %s: only lists and strings have items", type_name(target));
    }

    release(interp, index);
    release(interp, target);
    return result;
}

static value eval_assignment(interpreter* interp, const ASTNode* node){
    const ASTNode* target = node->data.binary_op_node.lhs;

    if (target->type == LEAF_NODE){
        value contents = eval(interp, node->data.binary_op_node.rhs);
        assign_variable(interp, target->data.value_node.identifier, contents, node);
        return retain(contents);
    }

    if (target->type == INDEX_NODE){
        value container = eval(interp, target->data.index_node.target);
        value index = eval(interp, target->data.index_node.index);
        value contents = eval(interp, node->data.binary_op_node.rhs);
        if (container.type == VAL_STRING){
            runtime_error(interp, target, "strings can't be changed in place: build a new one instead");
        }
        if (container.type != VAL_LIST){
            runtime_error(interp, target, "can't index %s: only lists and strings have items", type_name(container));
        }
        obj_list* list = AS_LIST(container);
        value* slot = &list->items[resolve_index(interp, index, list->count, target)];
        value old = *slot;
        *slot = retain(contents);
        release(interp, old);
        release(interp, index);
        release(interp, container);
        return contents;
    }

    runtime_error(interp, node, "can't assign to a field: LIPL values don't have fields");
}

static value eval_binary(interpreter* interp, const ASTNode* node){
    binary_op_node op = node->value.binary_op_node_value;
    const ASTNode* lhs = node->data.binary_op_node.lhs;
    const ASTNode* rhs = node->data.binary_op_node.rhs;

    switch (op){
        case ASSIGNMENT_NODE:
            return eval_assignment(interp, node);

        case AND_NODE:
        case OR_NODE: {
            value left = eval(interp, lhs);
            bool truth = is_truthy(left);
            release(interp, left);
            if (op == AND_NODE ? !truth : truth) return bool_value(truth);
            value right = eval(interp, rhs);
            truth = is_truthy(right);
            release(interp, right);
            return bool_value(truth);
        }

        case DOT_NODE: {
            if (rhs->type == FUNC_NODE){
                value receiver = eval(interp, lhs);
                return eval_call(interp, rhs, &receiver);
            }
            const char* member = name_of(interp, rhs->data.value_node.identifier);
            runtime_error(interp, node, "LIPL values don't have fields: did you mean '.%s()'?", member);
        }

        default: {
            value left = eval(interp, lhs);
            value right = eval(interp, rhs);
            value result = binary_operation(interp, op, left, right, node);
            release(interp, left);
            release(interp, right);
            return result;
        }
    }
}

static value eval_unary(interpreter* interp, const ASTNode* node){
    value operand = eval(interp, node->data.unary_op_node.operand);

    if (node->value.unary_op_node_value == NOT_NODE){
        bool truth = is_truthy(operand);
        release(interp, operand);
        return bool_value(!truth);
    }

    if (operand.type == VAL_INT){
        if (operand.as.integer == INT64_MIN) overflow_error(interp, node);
        return int_value(-operand.as.integer);
    }
    if (operand.type == VAL_FLOAT){
        return float_value(-operand.as.floating);
    }
    runtime_error(interp, node, "can't use '-' on %s", type_name(operand));
}

static value eval_list(interpreter* interp, const ASTNode* node){
    const statement_list* elements = node->data.list_node.elements;
    obj_list* list = new_list(interp, elements->count);
    value result = object_value(VAL_LIST, &list->header);
    for (uint32_t i = 0; i < elements->count; ++i){
        list_push(interp, list, eval(interp, elements->list[i]), node);
    }
    return result;
}

static value eval(interpreter* interp, const ASTNode* node){
    switch (node->type){
        case LEAF_NODE:         return eval_leaf(interp, node);
        case BINARY_OP_NODE:    return eval_binary(interp, node);
        case UNARY_OP_NODE:     return eval_unary(interp, node);
        case FUNC_NODE:         return eval_call(interp, node, NULL);
        case LIST_NODE:         return eval_list(interp, node);
        case INDEX_NODE:        return eval_index(interp, node);
        case RES_WORD_NODE:
        case DECLARATION_NODE:
        case CONDITIONAL_BLOCK_NODE:
        case NONE_AST_TYPE:
            break;
    }
    runtime_error(interp, node, "internal error: not an expression");
}

/* ---- statements ----------------------------------------------------------- */

static bool loop_should_continue(interpreter* interp, const ASTNode* condition){
    if (condition == NULL) return true;
    value result = eval(interp, condition);
    bool truth = is_truthy(result);
    release(interp, result);
    return truth;
}

static exec_status exec_while(interpreter* interp, const ASTNode* loop){
    while (loop_should_continue(interp, loop->data.conditional_block_node.condition)){
        exec_status status = exec_block(interp, loop->block);
        if (status == EXEC_BREAK) break;
        if (status == EXEC_RETURN) return EXEC_RETURN;
    }
    return EXEC_NORMAL;
}

static exec_status exec_for(interpreter* interp, const ASTNode* loop){
    const ASTNode* initialiser = loop->data.for_node.initialiser;
    scope* outer = interp->current;
    scope* loop_scope = NULL;

    /* `for var i = ...` gets its own scope so i doesn't leak out of the loop */
    if (initialiser != NULL && initialiser->type == DECLARATION_NODE){
        loop_scope = new_scope(interp, outer);
        interp->current = loop_scope;
    }
    if (initialiser != NULL){
        if (initialiser->type == DECLARATION_NODE){
            const ASTNode* first = initialiser->data.declaration_node.initialiser;
            declare(interp, initialiser->data.declaration_node.identifier,
                    first ? eval(interp, first) : nil_value(), 0, initialiser);
        }else{
            release(interp, eval(interp, initialiser));
        }
    }

    exec_status result = EXEC_NORMAL;
    while (loop_should_continue(interp, loop->data.for_node.condition)){
        exec_status status = exec_block(interp, loop->block);
        if (status == EXEC_BREAK) break;
        if (status == EXEC_RETURN){
            result = EXEC_RETURN;
            break;
        }
        if (loop->data.for_node.step != NULL){
            release(interp, eval(interp, loop->data.for_node.step));
        }
    }

    if (loop_scope != NULL){
        interp->current = outer;
        release_object(interp, &loop_scope->header);
    }
    return result;
}

static exec_status exec_if(interpreter* interp, const ASTNode* chain){
    for (const ASTNode* branch = chain; branch != NULL; branch = branch->data.conditional_block_node.else_branch){
        if (branch->value.conditional_block_node_value == ELSE_NODE){
            return exec_block(interp, branch->block);
        }
        value condition = eval(interp, branch->data.conditional_block_node.condition);
        bool taken = is_truthy(condition);
        release(interp, condition);
        if (taken){
            return exec_block(interp, branch->block);
        }
    }
    return EXEC_NORMAL;
}

static exec_status exec_statement(interpreter* interp, const ASTNode* node){
    switch (node->type){
        case DECLARATION_NODE: {
            const ASTNode* initialiser = node->data.declaration_node.initialiser;
            value contents = initialiser ? eval(interp, initialiser) : nil_value();
            declare(interp, node->data.declaration_node.identifier, contents, node->node_specifiers, node);
            return EXEC_NORMAL;
        }

        case FUNC_NODE:
            if (node->value.func_node_value == FUNC_DEC_NODE){
                obj_function* function = new_function(interp, node, interp->current);
                declare(interp, node->data.function_node.identifier,
                        object_value(VAL_FUNCTION, &function->header), node->node_specifiers, node);
                return EXEC_NORMAL;
            }
            break;  /* a call used as a statement */

        case RES_WORD_NODE:
            switch (node->value.reserved_word_value){
                case BREAK_NODE:    return EXEC_BREAK;
                case CONTINUE_NODE: return EXEC_CONTINUE;
                case RETURN_NODE: {
                    if (interp->call_depth == 0){
                        runtime_error(interp, node, "'return' outside a function");
                    }
                    const ASTNode* returned = node->data.return_block_node.value;
                    value result = returned ? eval(interp, returned) : nil_value();
                    release(interp, interp->return_value);
                    interp->return_value = result;
                    return EXEC_RETURN;
                }
            }
            break;

        case CONDITIONAL_BLOCK_NODE:
            switch (node->value.conditional_block_node_value){
                case IF_NODE:
                case ELIF_NODE:     return exec_if(interp, node);
                case ELSE_NODE:     return exec_block(interp, node->block);
                case WHILE_NODE:    return exec_while(interp, node);
                case FOR_NODE:      return exec_for(interp, node);
            }
            break;

        default:
            break;
    }

    release(interp, eval(interp, node));
    return EXEC_NORMAL;
}

static exec_status exec_statements(interpreter* interp, const statement_list* statements){
    for (uint32_t i = 0; i < statements->count; ++i){
        exec_status status = exec_statement(interp, statements->list[i]);
        if (status != EXEC_NORMAL) return status;
    }
    return EXEC_NORMAL;
}

/* only blocks that declare something need a scope of their own */
static bool declares_names(const statement_list* block){
    for (uint32_t i = 0; i < block->count; ++i){
        const ASTNode* statement = block->list[i];
        if (statement->type == DECLARATION_NODE) return true;
        if (statement->type == FUNC_NODE && statement->value.func_node_value == FUNC_DEC_NODE) return true;
    }
    return false;
}

static exec_status exec_block(interpreter* interp, const statement_list* block){
    if (!declares_names(block)){
        return exec_statements(interp, block);
    }
    scope* outer = interp->current;
    scope* inner = new_scope(interp, outer);
    interp->current = inner;
    exec_status status = exec_statements(interp, block);
    interp->current = outer;
    release_object(interp, &inner->header);
    return status;
}

/* ---- entry point ---------------------------------------------------------- */

int interpret(const ast_root* ast, symbol_table* symbols, const source_file* source){
    /* on the heap: locals changed between setjmp and longjmp aren't reliable afterwards */
    interpreter* interp = (interpreter*)safe_malloc(sizeof(interpreter));
    interp->symbols = symbols;
    interp->source = source;

    /* intern the builtin names first so the global tables cover them */
    int* builtin_ids = (int*)safe_malloc(sizeof(int) * builtin_count);
    for (size_t i = 0; i < builtin_count; ++i){
        builtin_ids[i] = intern_symbol(symbols, builtin_functions[i].name, strlen(builtin_functions[i].name));
    }
    interp->symbol_count = symbols->count;
    interp->globals = (binding*)safe_malloc(sizeof(binding) * interp->symbol_count);
    interp->builtins = (const builtin**)safe_malloc(sizeof(const builtin*) * interp->symbol_count);
    for (uint32_t id = 0; id < interp->symbol_count; ++id){
        interp->globals[id] = (binding){ .identifier = -1, .is_const = false, .contents = nil_value() };
        interp->builtins[id] = NULL;
    }
    for (size_t i = 0; i < builtin_count; ++i){
        interp->builtins[builtin_ids[i]] = &builtin_functions[i];
    }
    safe_free((void**)&builtin_ids);

    interp->current = NULL;
    interp->call_depth = 0;
    interp->return_value = nil_value();
    interp->objects = NULL;
    interp->free_queue = create_stack(sizeof(obj*));
    interp->arguments = NULL;
    interp->argument_count = 0;
    interp->argument_capacity = 0;

    size_t stack_size = thread_stack_size();
    interp->stack_base = stack_position();
    interp->stack_limit = stack_size ? (uintptr_t)(stack_size / 100u * STACK_USE_PERCENT) : FALLBACK_STACK_LIMIT;

    int status = 0;
    if (setjmp(interp->error_jump) == 0){
        exec_statements(interp, ast->statements);
    }else{
        status = 1;
    }
    fflush(stdout);

    free_all_objects(interp);
    destroy_stack(interp->free_queue);
    safe_free((void**)&interp->arguments);
    safe_free((void**)&interp->globals);
    safe_free((void**)&interp->builtins);
    safe_free((void**)&interp);
    return status;
}
