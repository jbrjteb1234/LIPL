#ifndef GRAMMAR
#define GRAMMAR

//different types of operator commands
typedef enum{
    SUBTRACTION,
    ADDITION,
    MULTIPLICATION,
    ASSIGNMENT,
    DIVISION,
    EQUIVALENT,
    NOT_EQUIVALENT,
    LESS_THAN,
    GREATER_THAN,
    GREATER_OR_EQUAL,
    LESS_OR_EQUAL,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_CBRACKET,
    CLOSE_CBRACKET
} operator_token;

//different types of reserved word commands
typedef enum{
    IF,
    ELSE
} reserved_word_token;

//describes the type of commands
typedef enum {
    RESERVED_WORD,
    OPERATOR,
    STRING_LITERAL,
    INT_VALUE,
    FLOAT_VALUE,
    IDENTIFIER
} token_types;

//  LEXEME      TOKEN VALUE         TOKEN TYPE      PRECEDENCE 
#define DEFINE_TOKENS \
    X("=",      ASSIGNMENT,         OPERATOR) \
    X("+",      ADDITION,           OPERATOR) \
    X("-",      SUBTRACTION,        OPERATOR) \
    X("*",      MULTIPLICATION,     OPERATOR) \
    X("/",      DIVISION,           OPERATOR) \
    X("<",      LESS_THAN,          OPERATOR) \
    X(">",      GREATER_THAN,       OPERATOR) \
    X("=<",     LESS_OR_EQUAL,      OPERATOR) \
    X(">=",     GREATER_OR_EQUAL,   OPERATOR) \
    X("==",     EQUIVALENT,         OPERATOR) \
    X("{",      OPEN_CBRACKET,      OPERATOR) \
    X("}",      CLOSE_CBRACKET,     OPERATOR) \
    X("(",      OPEN_BRACKET,       OPERATOR) \
    X(")",      CLOSE_BRACKET,      OPERATOR) \
    X("if",     IF,                 RESERVED_WORD) \
    X("else",   ELSE,               RESERVED_WORD) \
    X("while",  ELSE,               RESERVED_WORD) \
    X("return", ELSE,               RESERVED_WORD) \

#endif