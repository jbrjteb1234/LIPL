// interpreter/values.c - runtime objects, reference counting, formatting, equality
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"
#include "../lexer/lexer_main.h"
#include "../utility/safe_memory.h"

/* Lists nested deeper than this print as [...] and can't be compared */
#define MAX_VALUE_NESTING 1000u

/* ---- plain values --------------------------------------------------------- */

value nil_value(void){
    return (value){ .type = VAL_NIL, .as.integer = 0 };
}

value bool_value(bool boolean){
    return (value){ .type = VAL_BOOL, .as.boolean = boolean };
}

value int_value(int64_t integer){
    return (value){ .type = VAL_INT, .as.integer = integer };
}

value float_value(double floating){
    return (value){ .type = VAL_FLOAT, .as.floating = floating };
}

value object_value(value_type type, obj* object){
    return (value){ .type = type, .as.object = object };
}

bool is_object(value v){
    return v.type == VAL_STRING || v.type == VAL_LIST || v.type == VAL_FUNCTION;
}

bool is_truthy(value v){
    if (v.type == VAL_NIL) return false;
    if (v.type == VAL_BOOL) return v.as.boolean;
    return true;
}

const char* type_name(value v){
    switch (v.type){
        case VAL_NIL:       return "nil";
        case VAL_BOOL:      return "bool";
        case VAL_INT:       return "int";
        case VAL_FLOAT:     return "float";
        case VAL_STRING:    return "string";
        case VAL_LIST:      return "list";
        case VAL_FUNCTION:
        case VAL_BUILTIN:   return "function";
    }
    return "?";
}

/* ---- object lifetime ------------------------------------------------------ */

static obj* allocate_object(interpreter* interp, size_t size, obj_type type){
    obj* object = (obj*)safe_malloc(size);
    object->type = type;
    object->refcount = 1;
    object->prev = NULL;
    object->next = interp->objects;
    if (interp->objects){
        interp->objects->prev = object;
    }
    interp->objects = object;
    return object;
}

static void unlink_object(interpreter* interp, obj* object){
    if (object->prev){
        object->prev->next = object->next;
    }else{
        interp->objects = object->next;
    }
    if (object->next){
        object->next->prev = object->prev;
    }
}

/* frees an object's own memory without touching anything it refers to */
static void free_object_memory(obj* object){
    switch (object->type){
        case OBJ_LIST:      safe_free((void**)&((obj_list*)object)->items); break;
        case OBJ_SCOPE:     safe_free((void**)&((scope*)object)->bindings); break;
        case OBJ_STRING:
        case OBJ_FUNCTION:  break;
    }
    free(object);
}

value retain(value v){
    if (is_object(v)){
        v.as.object->refcount++;
    }
    return v;
}

static void drop_reference(interpreter* interp, obj* object){
    if (--object->refcount == 0){
        push(interp->free_queue, object, false);
    }
}

static void drop_value(interpreter* interp, value v){
    if (is_object(v)){
        drop_reference(interp, v.as.object);
    }
}

/* Freeing uses a work queue rather than recursion, so letting go of a list
   nested a million deep can't overflow the C stack. */
void release_object(interpreter* interp, obj* object){
    drop_reference(interp, object);

    void* top;
    while ((top = pop(interp->free_queue)) != NULL){
        obj* dead;
        memcpy(&dead, top, sizeof dead);

        switch (dead->type){
            case OBJ_LIST: {
                obj_list* list = (obj_list*)dead;
                for (uint32_t i = 0; i < list->count; ++i){
                    drop_value(interp, list->items[i]);
                }
                break;
            }
            case OBJ_FUNCTION: {
                obj_function* function = (obj_function*)dead;
                if (function->closure){
                    drop_reference(interp, &function->closure->header);
                }
                break;
            }
            case OBJ_SCOPE: {
                scope* variables = (scope*)dead;
                for (uint32_t i = 0; i < variables->count; ++i){
                    drop_value(interp, variables->bindings[i].contents);
                }
                if (variables->parent){
                    drop_reference(interp, &variables->parent->header);
                }
                break;
            }
            case OBJ_STRING:
                break;
        }

        unlink_object(interp, dead);
        free_object_memory(dead);
    }
}

void release(interpreter* interp, value v){
    if (is_object(v)){
        release_object(interp, v.as.object);
    }
}

void free_all_objects(interpreter* interp){
    obj* object = interp->objects;
    while (object != NULL){
        obj* next = object->next;
        free_object_memory(object);
        object = next;
    }
    interp->objects = NULL;
}

/* ---- object constructors -------------------------------------------------- */

obj_string* allocate_string(interpreter* interp, size_t length){
    if (length > MAX_STRING_LENGTH){
        fprintf(stderr, "fatal: string of %" PRIu64 " bytes is too long\n", (uint64_t)length);
        exit(EXIT_FAILURE);
    }
    obj_string* string = (obj_string*)allocate_object(interp, sizeof(obj_string) + length + 1u, OBJ_STRING);
    string->length = length;
    string->chars[length] = '\0';
    return string;
}

value make_string(interpreter* interp, const char* chars, size_t length){
    obj_string* string = allocate_string(interp, length);
    safe_memcpy(string->chars, chars, length);
    return object_value(VAL_STRING, &string->header);
}

obj_list* new_list(interpreter* interp, uint32_t capacity){
    obj_list* list = (obj_list*)allocate_object(interp, sizeof(obj_list), OBJ_LIST);
    list->count = 0;
    list->capacity = capacity ? capacity : 4u;
    list->visiting = false;
    list->items = (value*)safe_malloc(sizeof(value) * list->capacity);
    return list;
}

void list_push(interpreter* interp, obj_list* list, value item, const ASTNode* at){
    if (list->count == list->capacity){
        if (list->capacity > UINT32_MAX / 2u){
            runtime_error(interp, at, "list is too long");
        }
        list->capacity *= 2u;
        list->items = (value*)safe_realloc(list->items, sizeof(value) * list->capacity);
    }
    list->items[list->count++] = item;
}

obj_function* new_function(interpreter* interp, const ASTNode* declaration, scope* closure){
    obj_function* function = (obj_function*)allocate_object(interp, sizeof(obj_function), OBJ_FUNCTION);
    function->declaration = declaration;
    function->closure = closure;
    if (closure){
        closure->header.refcount++;
    }
    return function;
}

scope* new_scope(interpreter* interp, scope* parent){
    scope* variables = (scope*)allocate_object(interp, sizeof(scope), OBJ_SCOPE);
    variables->parent = parent;
    if (parent){
        parent->header.refcount++;
    }
    variables->count = 0;
    variables->capacity = 4;
    variables->bindings = (binding*)safe_malloc(sizeof(binding) * variables->capacity);
    return variables;
}

/* ---- equality ------------------------------------------------------------- */

static bool equal_at_depth(interpreter* interp, value a, value b, const ASTNode* at, uint32_t depth){
    bool a_number = a.type == VAL_INT || a.type == VAL_FLOAT;
    bool b_number = b.type == VAL_INT || b.type == VAL_FLOAT;
    if (a_number && b_number){
        if (a.type == VAL_INT && b.type == VAL_INT) return a.as.integer == b.as.integer;
        double x = a.type == VAL_INT ? (double)a.as.integer : a.as.floating;
        double y = b.type == VAL_INT ? (double)b.as.integer : b.as.floating;
        return x == y;
    }
    if (a.type != b.type) return false;

    switch (a.type){
        case VAL_NIL:       return true;
        case VAL_BOOL:      return a.as.boolean == b.as.boolean;
        case VAL_BUILTIN:   return a.as.native == b.as.native;
        case VAL_FUNCTION:  return a.as.object == b.as.object;
        case VAL_STRING: {
            obj_string* x = AS_STRING(a);
            obj_string* y = AS_STRING(b);
            return x->length == y->length && memcmp(x->chars, y->chars, x->length) == 0;
        }
        case VAL_LIST: {
            obj_list* x = AS_LIST(a);
            obj_list* y = AS_LIST(b);
            if (x == y) return true;
            if (x->count != y->count) return false;
            if (x->visiting || y->visiting) return false;   /* a cycle: only the same list equals itself */
            if (depth >= MAX_VALUE_NESTING){
                runtime_error(interp, at, "lists are nested too deeply to compare");
            }
            x->visiting = true;
            bool equal = true;
            for (uint32_t i = 0; i < x->count && equal; ++i){
                equal = equal_at_depth(interp, x->items[i], y->items[i], at, depth + 1u);
            }
            x->visiting = false;
            return equal;
        }
        case VAL_INT:
        case VAL_FLOAT:
            break;  /* handled above */
    }
    return false;
}

bool values_equal(interpreter* interp, value a, value b, const ASTNode* at){
    return equal_at_depth(interp, a, b, at, 0);
}

/* ---- text ----------------------------------------------------------------- */

void sb_init(string_builder* sb){
    sb->data = NULL;
    sb->length = 0;
    sb->capacity = 0;
}

void sb_append(string_builder* sb, const char* text, size_t length){
    if (length == 0) return;
    if (sb->length + length + 1u > sb->capacity){
        size_t capacity = sb->capacity ? sb->capacity : 32u;
        while (capacity < sb->length + length + 1u){
            capacity *= 2u;
        }
        sb->data = (char*)safe_realloc(sb->data, capacity);
        sb->capacity = capacity;
    }
    memcpy(sb->data + sb->length, text, length);
    sb->length += length;
    sb->data[sb->length] = '\0';
}

void sb_append_cstr(string_builder* sb, const char* text){
    sb_append(sb, text, strlen(text));
}

void sb_free(string_builder* sb){
    safe_free((void**)&sb->data);
    sb->length = 0;
    sb->capacity = 0;
}

value take_string(interpreter* interp, string_builder* sb){
    value result = make_string(interp, sb->data ? sb->data : "", sb->length);
    sb_free(sb);
    return result;
}

static void append_quoted(string_builder* out, const obj_string* string){
    sb_append(out, "\"", 1);
    for (size_t i = 0; i < string->length; ++i){
        char c = string->chars[i];
        switch (c){
            case '\n': sb_append(out, "\\n", 2);  break;
            case '\t': sb_append(out, "\\t", 2);  break;
            case '\r': sb_append(out, "\\r", 2);  break;
            case '"':  sb_append(out, "\\\"", 2); break;
            case '\\': sb_append(out, "\\\\", 2); break;
            default:   sb_append(out, &c, 1);     break;
        }
    }
    sb_append(out, "\"", 1);
}

static void format_at_depth(interpreter* interp, string_builder* out, value v, bool quote_strings, uint32_t depth){
    char number[64];
    switch (v.type){
        case VAL_NIL:
            sb_append_cstr(out, "nil");
            break;
        case VAL_BOOL:
            sb_append_cstr(out, v.as.boolean ? "true" : "false");
            break;
        case VAL_INT:
            snprintf(number, sizeof number, "%" PRId64, v.as.integer);
            sb_append_cstr(out, number);
            break;
        case VAL_FLOAT:
            format_float_literal(v.as.floating, number, sizeof number);
            sb_append_cstr(out, number);
            break;
        case VAL_STRING:
            if (quote_strings){
                append_quoted(out, AS_STRING(v));
            }else{
                sb_append(out, AS_STRING(v)->chars, AS_STRING(v)->length);
            }
            break;
        case VAL_LIST: {
            obj_list* list = AS_LIST(v);
            if (list->visiting || depth >= MAX_VALUE_NESTING){
                sb_append_cstr(out, "[...]");
                break;
            }
            list->visiting = true;
            sb_append(out, "[", 1);
            for (uint32_t i = 0; i < list->count; ++i){
                if (i > 0) sb_append(out, ", ", 2);
                format_at_depth(interp, out, list->items[i], true, depth + 1u);
            }
            sb_append(out, "]", 1);
            list->visiting = false;
            break;
        }
        case VAL_FUNCTION: {
            int name = AS_FUNCTION(v)->declaration->data.function_node.identifier;
            sb_append_cstr(out, "<func ");
            sb_append_cstr(out, symbol_name(interp->symbols, name));
            sb_append_cstr(out, ">");
            break;
        }
        case VAL_BUILTIN:
            sb_append_cstr(out, "<builtin ");
            sb_append_cstr(out, v.as.native->name);
            sb_append_cstr(out, ">");
            break;
    }
}

void format_value(interpreter* interp, string_builder* out, value v, bool quote_strings){
    format_at_depth(interp, out, v, quote_strings, 0);
}
