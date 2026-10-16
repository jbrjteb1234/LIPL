#ifndef LIPL_LEXER_TOKEN_H
#define LIPL_LEXER_TOKEN_H

#include <stdint.h>
#include "token_type.h"

//token structure - one command
typedef struct token{
    struct token* previous;
    struct token* next;

    token_types     token_type;
    token_values    token_value;

    /* 1-based position of the token's first character, for error messages */
    uint32_t line;
    uint32_t column;
    /* position just past its last character, for errors about what should follow it */
    uint32_t end_line;
    uint32_t end_column;
} token;

#endif /* LIPL_LEXER_TOKEN_H */
