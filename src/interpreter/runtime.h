#ifndef LIPL_INTERPRETER_RUNTIME_H
#define LIPL_INTERPRETER_RUNTIME_H

/*
 * Runtime values and interpreter state, shared by the evaluator (interpreter.c),
 * the value helpers (values.c) and the built-in functions (builtins.c).
 *
 * Memory: strings, lists, functions and scopes are reference counted, so each
 * is freed as soon as nothing refers to it. Every object is also kept on one
 * linked list, so whatever survives to the end - reference cycles, or values
 * abandoned by a runtime error - is freed in a single sweep.
 *
 * Ownership rules for `value`:
 *   - eval() and every builtin return a value the caller owns (one reference).
 *   - storing a value (variable, list slot) takes ownership of it.
 *   - retain() adds a reference, release() drops one.
 */

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../ast/ast_utility/ast.h"
#include "../lexer/symbol_table.h"
#include "../utility/stack.h"

typedef struct interpreter interpreter;
typedef struct obj obj;
typedef struct builtin builtin;
typedef struct scope scope;

typedef enum{
    VAL_NIL,
    VAL_BOOL,
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
    VAL_LIST,
    VAL_FUNCTION,
    VAL_BUILTIN,
} value_type;

typedef struct{
    value_type type;
    union{
        bool            boolean;
        int64_t         integer;
        double          floating;
        obj*            object;     /* VAL_STRING, VAL_LIST, VAL_FUNCTION */
        const builtin*  native;     /* VAL_BUILTIN */
    } as;
} value;

typedef enum{
    OBJ_STRING,
    OBJ_LIST,
    OBJ_FUNCTION,
    OBJ_SCOPE,
} obj_type;

struct obj{
    obj_type type;
    uint32_t refcount;
    obj*     prev;      /* the interpreter's list of every live object */
    obj*     next;
};

typedef struct{
    obj     header;
    size_t  length;
    char    chars[];    /* NUL-terminated, but may also contain NULs read from files */
} obj_string;

typedef struct{
    obj      header;
    uint32_t count;
    uint32_t capacity;
    bool     visiting;  /* set while printing or comparing, so a list that contains itself doesn't recurse forever */
    value*   items;
} obj_list;

typedef struct{
    int   identifier;   /* symbol id; -1 marks an unused global slot */
    bool  is_const;
    value contents;
} binding;

/* The variables of one function call or block */
struct scope{
    obj      header;
    scope*   parent;    /* NULL: the next scope out is the global table */
    uint32_t count;
    uint32_t capacity;
    binding* bindings;
};

typedef struct{
    obj             header;
    const ASTNode*  declaration;    /* the FUNC_DEC node: name, parameters, body */
    scope*          closure;        /* the scope it was declared in; NULL at top level */
} obj_function;

typedef value (*builtin_fn)(interpreter* interp, const value* args, uint32_t count, const ASTNode* call);

struct builtin{
    const char* name;
    uint32_t    min_args;
    uint32_t    max_args;
    builtin_fn  fn;
};

struct interpreter{
    symbol_table*   symbols;
    const char*     source_name;
    uint32_t        symbol_count;
    binding*        globals;        /* indexed by symbol id */
    const builtin** builtins;       /* indexed by symbol id; NULL where the name isn't a builtin */
    scope*          current;        /* innermost local scope; NULL at top level */
    uint32_t        call_depth;
    value           return_value;   /* set by `return`, taken by the call that returns */
    obj*            objects;        /* every live object */
    stack*          free_queue;     /* objects whose last reference went, waiting to be freed */

    /* arguments of the calls in progress, innermost last; owned references.
       Kept here rather than in C locals so each nested call costs less C stack. */
    value*          arguments;
    uint32_t        argument_count;
    uint32_t        argument_capacity;

    uintptr_t       stack_base;     /* address near the bottom of the C stack */
    uintptr_t       stack_limit;    /* how much of it calls may use before "too much recursion" */
    jmp_buf         error_jump;
};

#define AS_STRING(v)    ((obj_string*)(v).as.object)
#define AS_LIST(v)      ((obj_list*)(v).as.object)
#define AS_FUNCTION(v)  ((obj_function*)(v).as.object)

/* Longest string the runtime will build, so a runaway "x" * n fails cleanly */
#define MAX_STRING_LENGTH ((size_t)1 << 27)

/* ---- errors (interpreter.c) ----------------------------------------------- */

/* Prints "file:line:col: error: ..." and abandons the program. */
_Noreturn void runtime_error(interpreter* interp, const ASTNode* at, const char* format, ...);

/* ---- values (values.c) ---------------------------------------------------- */

value nil_value(void);
value bool_value(bool boolean);
value int_value(int64_t integer);
value float_value(double floating);
value object_value(value_type type, obj* object);

bool is_object(value v);
bool is_truthy(value v);            /* only false and nil are false */
const char* type_name(value v);

value retain(value v);
void release(interpreter* interp, value v);
void release_object(interpreter* interp, obj* object);

/* new objects start with one reference, owned by the caller */
obj_string* allocate_string(interpreter* interp, size_t length);    /* chars left for the caller to fill */
value make_string(interpreter* interp, const char* chars, size_t length);
obj_list* new_list(interpreter* interp, uint32_t capacity);
void list_push(interpreter* interp, obj_list* list, value item, const ASTNode* at);   /* takes ownership of item */
obj_function* new_function(interpreter* interp, const ASTNode* declaration, scope* closure);
scope* new_scope(interpreter* interp, scope* parent);

bool values_equal(interpreter* interp, value a, value b, const ASTNode* at);

/* Frees every remaining object, ignoring reference counts. Used at exit. */
void free_all_objects(interpreter* interp);

/* ---- text (values.c) ------------------------------------------------------ */

typedef struct{
    char*  data;
    size_t length;
    size_t capacity;
} string_builder;

void sb_init(string_builder* sb);
void sb_append(string_builder* sb, const char* text, size_t length);
void sb_append_cstr(string_builder* sb, const char* text);
void sb_free(string_builder* sb);

/* Writes v the way print() shows it. quote_strings puts strings in quotes (used inside lists). */
void format_value(interpreter* interp, string_builder* out, value v, bool quote_strings);

/* Turns the builder into a string value and frees the builder. */
value take_string(interpreter* interp, string_builder* sb);

/* ---- built-in functions (builtins.c) ------------------------------------ */

extern const builtin builtin_functions[];
extern const size_t builtin_count;

#endif /* LIPL_INTERPRETER_RUNTIME_H */
