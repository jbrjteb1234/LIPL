// lexer/lexer_main.c
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer_main.h"
#include "../grammar/grammar.h"
#include "../utility/diagnostics.h"
#include "../utility/safe_memory.h"

#define INITIAL_TOKEN_POOL_SIZE 64u

/* One entry per fixed lexeme listed in grammar.h */
typedef struct{
    const char*  lexeme;
    token_types  type;
    token_values value;
} grammar_entry;

static const grammar_entry grammar_entries[] = {
    #define X(LEXEME, VALUE, VALUE_TYPE, TYPE) { LEXEME, TYPE, { .VALUE_TYPE = VALUE } },
    DEFINE_TOKENS
    #undef X
};

#define GRAMMAR_ENTRY_COUNT (sizeof(grammar_entries) / sizeof(grammar_entries[0]))

static const char* const token_type_names[] = {
    [RESERVED_WORD]  = "RESERVED_WORD",
    [OPERATOR]       = "OPERATOR",
    [DELIMITER]      = "DELIMITER",
    [STRING_LITERAL] = "STRING_LITERAL",
    [INT_VALUE]      = "INT_VALUE",
    [IDENTIFIER]     = "IDENTIFIER",
    [FLOAT_VALUE]    = "FLOAT_VALUE",
    [END_OF_INPUT]   = "END_OF_INPUT",
};

/* Cursor over the source text */
typedef struct{
    const char*     source;
    size_t          length;
    size_t          position;
    uint32_t        line;
    uint32_t        column;
    token_stream*   stream;
    symbol_table*   symbols;
} scanner;

/* ---- scanner primitives --------------------------------------------------- */

/* Returns the character `offset` places ahead as an unsigned char, or EOF */
static int peek_char(const scanner* sc, size_t offset){
    size_t at = sc->position + offset;
    return at < sc->length ? (unsigned char)sc->source[at] : EOF;
}

static void advance_char(scanner* sc){
    if (sc->position >= sc->length) return;
    if (sc->source[sc->position] == '\n'){
        sc->line++;
        sc->column = 1;
    }else{
        sc->column++;
    }
    sc->position++;
}

static bool starts_with(const scanner* sc, const char* text){
    size_t n = strlen(text);
    return sc->length - sc->position >= n && memcmp(sc->source + sc->position, text, n) == 0;
}

static bool is_digit(int c){
    return c != EOF && isdigit(c);
}

static bool is_word_char(int c){
    return c != EOF && (isalnum(c) || c == '_');
}

static void lex_error(scanner* sc, uint32_t line, uint32_t column, const char* format, ...){
    va_list args;
    va_start(args, format);
    report_error(sc->stream->source, line, column, format, args);
    va_end(args);
    sc->stream->error_count++;
}

static token* emit_token(scanner* sc, token_types type, uint32_t line, uint32_t column){
    token_stream* stream = sc->stream;
    token* new_token = (token*)acquire_from_pool(stream->pool);  /* zero-filled */
    new_token->token_type = type;
    new_token->line = line;
    new_token->column = column;
    new_token->previous = stream->tail;
    if (stream->tail){
        stream->tail->next = new_token;
    }else{
        stream->head = new_token;
    }
    stream->tail = new_token;
    return new_token;
}

static const grammar_entry* find_fixed_lexeme(const char* text, size_t length){
    for (size_t i = 0; i < GRAMMAR_ENTRY_COUNT; ++i){
        const char* lexeme = grammar_entries[i].lexeme;
        if (strlen(lexeme) == length && memcmp(lexeme, text, length) == 0){
            return &grammar_entries[i];
        }
    }
    return NULL;
}

/* ---- token scanners ------------------------------------------------------- */

static void skip_whitespace_and_comments(scanner* sc){
    for (;;){
        int c = peek_char(sc, 0);
        if (c != EOF && isspace(c)){
            advance_char(sc);
        }else if (starts_with(sc, LINE_COMMENT)){
            while (peek_char(sc, 0) != EOF && peek_char(sc, 0) != '\n'){
                advance_char(sc);
            }
        }else{
            return;
        }
    }
}

/* identifiers, reserved words, and the lone "_" root operator */
static void scan_word(scanner* sc, uint32_t line, uint32_t column){
    size_t start = sc->position;
    while (is_word_char(peek_char(sc, 0))){
        advance_char(sc);
    }
    const char* text = sc->source + start;
    size_t length = sc->position - start;

    const grammar_entry* entry = find_fixed_lexeme(text, length);
    if (entry){
        token* fixed = emit_token(sc, entry->type, line, column);
        fixed->token_value = entry->value;
        return;
    }

    token* identifier = emit_token(sc, IDENTIFIER, line, column);
    identifier->token_value.identifier_token_value = intern_symbol(sc->symbols, text, length);
}

/* integers (42) and decimals (3.14). "3.foo" is 3 followed by member access. */
static void scan_number(scanner* sc, uint32_t line, uint32_t column){
    size_t start = sc->position;
    bool is_float = false;

    while (is_digit(peek_char(sc, 0))){
        advance_char(sc);
    }
    if (peek_char(sc, 0) == '.' && is_digit(peek_char(sc, 1))){
        is_float = true;
        advance_char(sc);
        while (is_digit(peek_char(sc, 0))){
            advance_char(sc);
        }
    }

    if (is_word_char(peek_char(sc, 0))){
        while (is_word_char(peek_char(sc, 0))){
            advance_char(sc);
        }
        lex_error(sc, line, column, "invalid number '%.*s'",
                  (int)(sc->position - start), sc->source + start);
        return;
    }

    char* text = safe_strndup(sc->source + start, sc->position - start);
    errno = 0;
    if (is_float){
        double value = strtod(text, NULL);
        if (errno == ERANGE){
            lex_error(sc, line, column, "number '%s' is out of range", text);
        }else{
            emit_token(sc, FLOAT_VALUE, line, column)->token_value.float_value = value;
        }
    }else{
        long long value = strtoll(text, NULL, 10);
        if (errno == ERANGE){
            lex_error(sc, line, column, "integer '%s' is too large", text);
        }else{
            emit_token(sc, INT_VALUE, line, column)->token_value.int_value = (int64_t)value;
        }
    }
    safe_free((void**)&text);
}

/* "..." with \n \t \r \" \\ escapes; the token holds the unescaped text */
static void scan_string(scanner* sc, uint32_t line, uint32_t column){
    advance_char(sc); /* opening quote */

    size_t capacity = 16;
    size_t length = 0;
    char* value = (char*)safe_malloc(capacity);
    bool valid = true;

    for (;;){
        int c = peek_char(sc, 0);
        if (c == EOF){
            lex_error(sc, line, column, "unterminated string literal");
            safe_free((void**)&value);
            return;
        }
        uint32_t char_line = sc->line;
        uint32_t char_column = sc->column;
        advance_char(sc);

        if (c == '"'){
            break;
        }
        if (c == '\\'){
            int escaped = peek_char(sc, 0);
            if (escaped == EOF){
                continue; /* reported as unterminated on the next pass */
            }
            advance_char(sc);
            switch (escaped){
                case 'n':  c = '\n'; break;
                case 't':  c = '\t'; break;
                case 'r':  c = '\r'; break;
                case '"':  c = '"';  break;
                case '\\': c = '\\'; break;
                default:
                    lex_error(sc, char_line, char_column, "unknown escape sequence '\\%c'", escaped);
                    valid = false;
                    continue;
            }
        }

        if (length + 1u >= capacity){
            capacity *= 2u;
            value = (char*)safe_realloc(value, capacity);
        }
        value[length++] = (char)c;
    }

    value[length] = '\0';
    if (!valid){
        safe_free((void**)&value);
        return;
    }
    emit_token(sc, STRING_LITERAL, line, column)->token_value.string_value = value;
}

/* operators and delimiters: longest match against grammar.h */
static void scan_symbol(scanner* sc, uint32_t line, uint32_t column){
    const grammar_entry* best = NULL;
    size_t best_length = 0;

    for (size_t i = 0; i < GRAMMAR_ENTRY_COUNT; ++i){
        const grammar_entry* entry = &grammar_entries[i];
        size_t length = strlen(entry->lexeme);
        if (length > best_length && !is_word_char((unsigned char)entry->lexeme[0]) && starts_with(sc, entry->lexeme)){
            best = entry;
            best_length = length;
        }
    }

    if (!best){
        int c = peek_char(sc, 0);
        if (isprint(c)){
            lex_error(sc, line, column, "unexpected character '%c'", c);
        }else{
            lex_error(sc, line, column, "unexpected byte 0x%02X", (unsigned)c);
        }
        advance_char(sc);
        return;
    }

    for (size_t i = 0; i < best_length; ++i){
        advance_char(sc);
    }
    emit_token(sc, best->type, line, column)->token_value = best->value;
}

/* ---- public interface ----------------------------------------------------- */

token_stream* tokenize(const source_file* source, symbol_table* symbols){
    token_stream* stream = (token_stream*)safe_malloc(sizeof(token_stream));
    stream->head = NULL;
    stream->tail = NULL;
    stream->pool = init_data_pool(sizeof(token), INITIAL_TOKEN_POOL_SIZE);
    stream->source = source;
    stream->error_count = 0;

    scanner sc = {
        .source = source->text, .length = source->length, .position = 0,
        .line = 1, .column = 1,
        .stream = stream, .symbols = symbols,
    };

    for (;;){
        skip_whitespace_and_comments(&sc);
        uint32_t line = sc.line;
        uint32_t column = sc.column;
        int c = peek_char(&sc, 0);

        if (c == EOF){
            emit_token(&sc, END_OF_INPUT, line, column);
            break;
        }
        if (isalpha(c) || c == '_'){
            scan_word(&sc, line, column);
        }else if (isdigit(c)){
            scan_number(&sc, line, column);
        }else if (c == '"'){
            scan_string(&sc, line, column);
        }else{
            scan_symbol(&sc, line, column);
        }
    }

    return stream;
}

void destroy_token_stream(token_stream* stream){
    if (!stream) return;
    /* free per-token owned payloads */
    for (token* t = stream->head; t != NULL; t = t->next){
        if (t->token_type == STRING_LITERAL){
            safe_free((void**)&t->token_value.string_value);
        }
    }
    shutdown_data_pool(stream->pool);
    safe_free((void**)&stream);
}

const char* fixed_lexeme(token_types type, token_values value){
    for (size_t i = 0; i < GRAMMAR_ENTRY_COUNT; ++i){
        const grammar_entry* entry = &grammar_entries[i];
        if (entry->type != type) continue;
        switch (type){
            case OPERATOR:
                if (entry->value.operator_token_value == value.operator_token_value) return entry->lexeme;
                break;
            case DELIMITER:
                if (entry->value.delimiter_token_value == value.delimiter_token_value) return entry->lexeme;
                break;
            case RESERVED_WORD:
                if (entry->value.reserved_word_token_value == value.reserved_word_token_value) return entry->lexeme;
                break;
            default:
                return NULL;
        }
    }
    return NULL;
}

const char* operator_lexeme(operator_token op){
    token_values value = { .operator_token_value = op };
    return fixed_lexeme(OPERATOR, value);
}

void format_float_literal(double value, char* buffer, size_t size){
    snprintf(buffer, size, "%.15g", value);
    /* "n" catches inf and nan, which shouldn't get ".0" appended */
    if (strpbrk(buffer, ".eEn") == NULL){
        size_t used = strlen(buffer);
        if (used + 2u < size){
            buffer[used] = '.';
            buffer[used + 1u] = '0';
            buffer[used + 2u] = '\0';
        }
    }
}

void describe_token(const token* t, const symbol_table* symbols, char* buffer, size_t size){
    switch (t->token_type){
        case END_OF_INPUT:
            snprintf(buffer, size, "end of file");
            break;
        case IDENTIFIER:
            snprintf(buffer, size, "identifier '%s'", symbol_name(symbols, t->token_value.identifier_token_value));
            break;
        case INT_VALUE:
            snprintf(buffer, size, "number %" PRId64, t->token_value.int_value);
            break;
        case FLOAT_VALUE: {
            char number[48];
            format_float_literal(t->token_value.float_value, number, sizeof number);
            snprintf(buffer, size, "number %s", number);
            break;
        }
        case STRING_LITERAL:
            snprintf(buffer, size, "a string");
            break;
        default: {
            const char* lexeme = fixed_lexeme(t->token_type, t->token_value);
            snprintf(buffer, size, "'%s'", lexeme ? lexeme : "?");
            break;
        }
    }
}

void print_string_literal(FILE* out, const char* text){
    fputc('"', out);
    for (const char* c = text; *c != '\0'; ++c){
        switch (*c){
            case '\n': fputs("\\n", out);  break;
            case '\t': fputs("\\t", out);  break;
            case '\r': fputs("\\r", out);  break;
            case '"':  fputs("\\\"", out); break;
            case '\\': fputs("\\\\", out); break;
            default:   fputc(*c, out);     break;
        }
    }
    fputc('"', out);
}

void print_token_stream(FILE* out, const token_stream* stream, const symbol_table* symbols) {
    for (const token* t = stream->head; t != NULL; t = t->next) {
        char position[24];
        snprintf(position, sizeof position, "%u:%u", (unsigned)t->line, (unsigned)t->column);
        fprintf(out, "%-9s %-15s", position, token_type_names[t->token_type]);

        switch (t->token_type){
            case IDENTIFIER:
                fputs(symbol_name(symbols, t->token_value.identifier_token_value), out);
                break;
            case INT_VALUE:
                fprintf(out, "%" PRId64, t->token_value.int_value);
                break;
            case FLOAT_VALUE: {
                char number[48];
                format_float_literal(t->token_value.float_value, number, sizeof number);
                fputs(number, out);
                break;
            }
            case STRING_LITERAL:
                print_string_literal(out, t->token_value.string_value);
                break;
            case END_OF_INPUT:
                break;
            default: {
                const char* lexeme = fixed_lexeme(t->token_type, t->token_value);
                fputs(lexeme ? lexeme : "?", out);
                break;
            }
        }
        fputc('\n', out);
    }
}
