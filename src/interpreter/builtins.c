// interpreter/builtins.c - the functions every program starts with
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"
#include "../utility/safe_memory.h"

/* ---- helpers -------------------------------------------------------------- */

static _Noreturn void wrong_type(interpreter* interp, const ASTNode* call, const char* function, const char* wanted, value got){
    runtime_error(interp, call, "%s() needs %s, not %s", function, wanted, type_name(got));
}

static obj_string* expect_string(interpreter* interp, const ASTNode* call, const char* function, value v){
    if (v.type != VAL_STRING) wrong_type(interp, call, function, "a string", v);
    return AS_STRING(v);
}

static obj_list* expect_list(interpreter* interp, const ASTNode* call, const char* function, value v){
    if (v.type != VAL_LIST) wrong_type(interp, call, function, "a list", v);
    return AS_LIST(v);
}

/* the string with leading and trailing whitespace removed, as [*start, *start + *length) */
static void trim(const obj_string* string, const char** start, size_t* length){
    const char* begin = string->chars;
    const char* end = string->chars + string->length;
    while (begin < end && isspace((unsigned char)*begin)) begin++;
    while (end > begin && isspace((unsigned char)end[-1])) end--;
    *start = begin;
    *length = (size_t)(end - begin);
}

static _Noreturn void bad_conversion(interpreter* interp, const ASTNode* call, const char* function, const char* target, value v){
    string_builder shown;
    sb_init(&shown);
    format_value(interp, &shown, v, true);
    /* copy into a fixed buffer so the builder is freed before runtime_error jumps away */
    char text[80];
    snprintf(text, sizeof text, "%s", shown.data ? shown.data : "");
    sb_free(&shown);
    runtime_error(interp, call, "%s() can't turn %s into %s", function, text, target);
}

/* ---- print and input ------------------------------------------------------ */

static value builtin_print(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)call;
    string_builder line;
    sb_init(&line);
    for (uint32_t i = 0; i < count; ++i){
        if (i > 0) sb_append(&line, " ", 1);
        format_value(interp, &line, args[i], false);
    }
    sb_append(&line, "\n", 1);
    fwrite(line.data, 1, line.length, stdout);
    sb_free(&line);
    return nil_value();
}

static value builtin_input(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)call;
    if (count == 1){
        string_builder prompt;
        sb_init(&prompt);
        format_value(interp, &prompt, args[0], false);
        if (prompt.length) fwrite(prompt.data, 1, prompt.length, stdout);
        sb_free(&prompt);
    }
    fflush(stdout);

    string_builder line;
    sb_init(&line);
    bool read_anything = false;
    int c;
    while ((c = fgetc(stdin)) != EOF){
        read_anything = true;
        if (c == '\n') break;
        char ch = (char)c;
        sb_append(&line, &ch, 1);
    }
    if (!read_anything){
        sb_free(&line);
        return nil_value();     /* end of input */
    }
    if (line.length > 0 && line.data[line.length - 1] == '\r'){
        line.length--;
    }
    return take_string(interp, &line);
}

/* ---- lists and strings ---------------------------------------------------- */

static value builtin_len(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    if (args[0].type == VAL_STRING) return int_value((int64_t)AS_STRING(args[0])->length);
    if (args[0].type == VAL_LIST) return int_value((int64_t)AS_LIST(args[0])->count);
    wrong_type(interp, call, "len", "a string or a list", args[0]);
}

static value builtin_push(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    obj_list* list = expect_list(interp, call, "push", args[0]);
    list_push(interp, list, retain(args[1]), call);
    return nil_value();
}

static value builtin_pop(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    obj_list* list = expect_list(interp, call, "pop", args[0]);
    if (list->count == 0){
        runtime_error(interp, call, "pop() on an empty list");
    }
    return list->items[--list->count];     /* the list's reference passes to the caller */
}

/* split(s) splits on runs of whitespace; split(s, sep) splits on every sep */
static value builtin_split(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    obj_string* text = expect_string(interp, call, "split", args[0]);
    obj_string* separator = NULL;
    if (count == 2){
        separator = expect_string(interp, call, "split", args[1]);
        if (separator->length == 0){
            runtime_error(interp, call, "split() needs a separator that isn't empty");
        }
    }

    obj_list* parts = new_list(interp, 0);
    value result = object_value(VAL_LIST, &parts->header);
    const char* chars = text->chars;
    size_t length = text->length;

    if (separator == NULL){
        size_t i = 0;
        while (i < length){
            while (i < length && isspace((unsigned char)chars[i])) i++;
            size_t start = i;
            while (i < length && !isspace((unsigned char)chars[i])) i++;
            if (i > start){
                list_push(interp, parts, make_string(interp, chars + start, i - start), call);
            }
        }
        return result;
    }

    size_t start = 0;
    size_t i = 0;
    while (i + separator->length <= length){
        if (memcmp(chars + i, separator->chars, separator->length) == 0){
            list_push(interp, parts, make_string(interp, chars + start, i - start), call);
            i += separator->length;
            start = i;
        }else{
            i++;
        }
    }
    list_push(interp, parts, make_string(interp, chars + start, length - start), call);
    return result;
}

/* ---- conversions ---------------------------------------------------------- */

static value builtin_str(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    (void)call;
    if (args[0].type == VAL_STRING) return retain(args[0]);
    string_builder text;
    sb_init(&text);
    format_value(interp, &text, args[0], false);
    return take_string(interp, &text);
}

static value builtin_int(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    value v = args[0];
    switch (v.type){
        case VAL_INT:
            return v;
        case VAL_BOOL:
            return int_value(v.as.boolean ? 1 : 0);
        case VAL_FLOAT:
            /* the range check also rejects inf; the comparison is exact because 2^63 is a double */
            if (!(v.as.floating > -9223372036854775808.0 && v.as.floating < 9223372036854775808.0)){
                bad_conversion(interp, call, "int", "an integer", v);
            }
            return int_value((int64_t)v.as.floating);
        case VAL_STRING: {
            const char* start;
            size_t length;
            trim(AS_STRING(v), &start, &length);
            size_t i = (length > 0 && (start[0] == '-' || start[0] == '+')) ? 1 : 0;
            if (i == length) bad_conversion(interp, call, "int", "an integer", v);
            for (size_t j = i; j < length; ++j){
                if (!isdigit((unsigned char)start[j])) bad_conversion(interp, call, "int", "an integer", v);
            }
            char* digits = safe_strndup(start, length);
            errno = 0;
            long long parsed = strtoll(digits, NULL, 10);
            bool out_of_range = errno == ERANGE;
            safe_free((void**)&digits);
            if (out_of_range) bad_conversion(interp, call, "int", "an integer (it's too big)", v);
            return int_value((int64_t)parsed);
        }
        default:
            bad_conversion(interp, call, "int", "an integer", v);
    }
}

static value builtin_float(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    value v = args[0];
    switch (v.type){
        case VAL_FLOAT:
            return v;
        case VAL_INT:
            return float_value((double)v.as.integer);
        case VAL_BOOL:
            return float_value(v.as.boolean ? 1.0 : 0.0);
        case VAL_STRING: {
            const char* start;
            size_t length;
            trim(AS_STRING(v), &start, &length);
            /* plain decimal notation only: strtod would also take "nan", "inf" and hex */
            bool has_digit = false;
            for (size_t j = 0; j < length; ++j){
                char c = start[j];
                if (isdigit((unsigned char)c)) has_digit = true;
                else if (c != '+' && c != '-' && c != '.' && c != 'e' && c != 'E') bad_conversion(interp, call, "float", "a number", v);
            }
            if (!has_digit) bad_conversion(interp, call, "float", "a number", v);
            char* text = safe_strndup(start, length);
            char* end = NULL;
            double parsed = strtod(text, &end);
            bool complete = end == text + length;
            safe_free((void**)&text);
            if (!complete || __builtin_isnan(parsed)) bad_conversion(interp, call, "float", "a number", v);
            return float_value(parsed);
        }
        default:
            bad_conversion(interp, call, "float", "a number", v);
    }
}

static value builtin_type(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    (void)call;
    const char* name = type_name(args[0]);
    return make_string(interp, name, strlen(name));
}

/* ---- files ---------------------------------------------------------------- */

/* Every line of a text file, without line endings. */
static value builtin_read_lines(interpreter* interp, const value* args, uint32_t count, const ASTNode* call){
    (void)count;
    obj_string* path = expect_string(interp, call, "read_lines", args[0]);
    FILE* file = fopen(path->chars, "rb");
    if (file == NULL){
        runtime_error(interp, call, "read_lines() can't open \"%s\": %s", path->chars, strerror(errno));
    }

    string_builder contents;
    sb_init(&contents);
    char chunk[4096];
    size_t got;
    while ((got = fread(chunk, 1, sizeof chunk, file)) > 0){
        sb_append(&contents, chunk, got);
        if (contents.length > MAX_STRING_LENGTH) break;
    }
    bool failed = ferror(file) != 0;
    fclose(file);
    if (failed || contents.length > MAX_STRING_LENGTH){
        sb_free(&contents);
        runtime_error(interp, call, "read_lines() couldn't read \"%s\"", path->chars);
    }

    obj_list* lines = new_list(interp, 0);
    size_t start = 0;
    for (size_t i = 0; i <= contents.length; ++i){
        bool at_end = i == contents.length;
        if (!at_end && contents.data[i] != '\n') continue;
        if (at_end && i == start) break;    /* no empty line after a final newline */
        size_t end = i;
        if (end > start && contents.data[end - 1] == '\r') end--;
        list_push(interp, lines, make_string(interp, contents.data + start, end - start), call);
        start = i + 1;
    }
    sb_free(&contents);
    return object_value(VAL_LIST, &lines->header);
}

/* ---- table ---------------------------------------------------------------- */

const builtin builtin_functions[] = {
    { "print",      0, UINT32_MAX,  builtin_print },
    { "input",      0, 1,           builtin_input },
    { "len",        1, 1,           builtin_len },
    { "push",       2, 2,           builtin_push },
    { "pop",        1, 1,           builtin_pop },
    { "split",      1, 2,           builtin_split },
    { "str",        1, 1,           builtin_str },
    { "int",        1, 1,           builtin_int },
    { "float",      1, 1,           builtin_float },
    { "type",       1, 1,           builtin_type },
    { "read_lines", 1, 1,           builtin_read_lines },
};

const size_t builtin_count = sizeof(builtin_functions) / sizeof(builtin_functions[0]);
