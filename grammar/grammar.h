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
    ELSE,
    WHILE,
    RETURN
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

#define IDENTIFIER_PRECEDENCE   254
#define LITERAL_PRECEDENCE      255

//  LEXEME      TOKEN VALUE         TOKEN TYPE      PRECEDENCE 
#define DEFINE_TOKENS \
    X("=",      ASSIGNMENT,         OPERATOR,       10) \
    X("+",      ADDITION,           OPERATOR,       6)  \
    X("-",      SUBTRACTION,        OPERATOR,       6)  \
    X("*",      MULTIPLICATION,     OPERATOR,       5)  \
    X("/",      DIVISION,           OPERATOR,       5)  \
    X("<",      LESS_THAN,          OPERATOR,       8)  \
    X(">",      GREATER_THAN,       OPERATOR,       8)  \
    X("=<",     LESS_OR_EQUAL,      OPERATOR,       8)  \
    X(">=",     GREATER_OR_EQUAL,   OPERATOR,       8)  \
    X("==",     EQUIVALENT,         OPERATOR,       9)  \
    X("{",      OPEN_CBRACKET,      OPERATOR,       1)  \
    X("}",      CLOSE_CBRACKET,     OPERATOR,       1)  \
    X("(",      OPEN_BRACKET,       OPERATOR,       1)  \
    X(")",      CLOSE_BRACKET,      OPERATOR,       1)  \
    X("if",     IF,                 RESERVED_WORD,  0)  \
    X("else",   ELSE,               RESERVED_WORD,  0)  \
    X("while",  WHILE,              RESERVED_WORD,  0)  \
    X("return", RETURN,             RESERVED_WORD,  0)

#endif