// utility/diagnostics.c - error messages that quote the line they're about
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diagnostics.h"
#include "platform.h"

/* A tab in a quoted line is shown as this many spaces, so the caret line can
   be padded to match it exactly */
#define TAB_WIDTH 4u

/* A longer line is cut down to this many bytes around the caret */
#define MAX_QUOTED_LENGTH 96u

#define COLOUR_BOLD     "\033[1m"
#define COLOUR_RED      "\033[1;31m"
#define COLOUR_GREEN    "\033[1;32m"
#define COLOUR_RESET    "\033[0m"

static bool use_colour(void){
    static int decided = -1;
    if (decided < 0){
        /* https://no-color.org */
        const char* no_colour = getenv("NO_COLOR");
        decided = (no_colour == NULL || no_colour[0] == '\0') && stderr_supports_colour();
    }
    return decided != 0;
}

static bool is_utf8_continuation(char c){
    return ((unsigned char)c & 0xC0u) == 0x80u;
}

/* How many columns a byte of a quoted line takes up on screen */
static unsigned display_width(char c){
    if (c == '\t') return TAB_WIDTH;
    if (is_utf8_continuation(c)) return 0;     /* part of the character before it */
    return 1;
}

static void put_quoted_byte(char c){
    unsigned char byte = (unsigned char)c;
    if (c == '\t'){
        fprintf(stderr, "%*s", (int)TAB_WIDTH, "");
    }else if (byte < 0x20u || byte == 0x7Fu){
        fputc('?', stderr);     /* a control character would garble the terminal */
    }else{
        fputc(c, stderr);
    }
}

/* Finds 1-based line `line`: returns its start and sets *line_end to just
   before its '\n' (or "\r\n"). Returns NULL if the text has fewer lines. */
static const char* find_line(const source_file* source, uint32_t line, const char** line_end){
    const char* text_end = source->text + source->length;
    const char* start = source->text;
    for (uint32_t current = 1; current < line; ++current){
        const char* newline = memchr(start, '\n', (size_t)(text_end - start));
        if (newline == NULL) return NULL;
        start = newline + 1;
    }

    const char* end = memchr(start, '\n', (size_t)(text_end - start));
    if (end == NULL) end = text_end;
    if (end > start && end[-1] == '\r') end--;
    *line_end = end;
    return start;
}

/*     3 | x = 1 + 2
         |          ^          */
static void quote_source_line(const source_file* source, uint32_t line, uint32_t column, bool colour){
    const char* line_end = NULL;
    const char* text = find_line(source, line, &line_end);
    if (text == NULL) return;

    size_t length = (size_t)(line_end - text);
    size_t caret = column > 0 ? column - 1u : 0;    /* byte offset of the caret in the line */
    if (caret > length) caret = length;

    /* a long line is cut down to the part around the caret, with "..." where it was cut */
    size_t shown_start = 0;
    size_t shown_end = length;
    if (length > MAX_QUOTED_LENGTH){
        shown_start = caret > MAX_QUOTED_LENGTH / 2u ? caret - MAX_QUOTED_LENGTH / 2u : 0;
        if (shown_start > length - MAX_QUOTED_LENGTH) shown_start = length - MAX_QUOTED_LENGTH;
        shown_end = shown_start + MAX_QUOTED_LENGTH;
        while (shown_start > 0 && is_utf8_continuation(text[shown_start])) shown_start--;
        while (shown_end < length && is_utf8_continuation(text[shown_end])) shown_end++;
    }

    char number[16];
    int gutter_width = snprintf(number, sizeof number, "%5u", (unsigned)line);

    fprintf(stderr, "%s |", number);
    if (length > 0) fputc(' ', stderr);
    if (shown_start > 0) fputs("...", stderr);
    for (size_t i = shown_start; i < shown_end; ++i){
        put_quoted_byte(text[i]);
    }
    if (shown_end < length) fputs("...", stderr);
    fputc('\n', stderr);

    unsigned padding = shown_start > 0 ? 3u : 0u;
    for (size_t i = shown_start; i < caret; ++i){
        padding += display_width(text[i]);
    }
    fprintf(stderr, "%*s | %*s", gutter_width, "", (int)padding, "");
    fputs(colour ? COLOUR_GREEN "^" COLOUR_RESET "\n" : "^\n", stderr);
}

void report_error(const source_file* source, uint32_t line, uint32_t column, const char* format, va_list args){
    bool colour = use_colour();
    if (colour) fputs(COLOUR_BOLD, stderr);
    fprintf(stderr, "%s:%u:%u: ", source->name, (unsigned)line, (unsigned)column);
    fputs(colour ? COLOUR_RED "error:" COLOUR_RESET COLOUR_BOLD " " : "error: ", stderr);
    vfprintf(stderr, format, args);
    if (colour) fputs(COLOUR_RESET, stderr);
    fputc('\n', stderr);

    quote_source_line(source, line, column, colour);
}
