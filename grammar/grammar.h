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
} operator_token;

//different types of reserved word commands
typedef enum{
    IF,
    ELSE,
    WHILE,
    RETURN
} reserved_word_token;

typedef enum{
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_CBRACKET,
    CLOSE_CBRACKET,
    EOS
} delimiter_token;

//The token value - could be an enum for operators or reserved words, or a variable value
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    delimiter_token         delimiter_token_value;
    int*                    number_token_value;
    char *                  string_literal_token_value;
    int                     identifier_token_value;                 
} token_values;

//describes the type of commands
typedef enum {
    RESERVED_WORD,
    OPERATOR,
    DELIMITER,
    STRING_LITERAL,
    INT_VALUE,
    IDENTIFIER
} token_types;

#define IDENTIFIER_PRECEDENCE   254
#define LITERAL_PRECEDENCE      255

#define DELIMITER_SYMBOL "!"

//  LEXEME              TOKEN VALUE         TOKEN TYPE      PRECEDENCE 
#define DEFINE_TOKENS \
    X("=",              ASSIGNMENT,         OPERATOR,       10) \
    X("==",             EQUIVALENT,         OPERATOR,       9)  \
    X("<",              LESS_THAN,          OPERATOR,       8)  \
    X(">",              GREATER_THAN,       OPERATOR,       8)  \
    X("=<",             LESS_OR_EQUAL,      OPERATOR,       8)  \
    X(">=",             GREATER_OR_EQUAL,   OPERATOR,       8)  \
    X("+",              ADDITION,           OPERATOR,       6)  \
    X("-",              SUBTRACTION,        OPERATOR,       6)  \
    X("*",              MULTIPLICATION,     OPERATOR,       5)  \
    X("/",              DIVISION,           OPERATOR,       5)  \
    X("{",              OPEN_CBRACKET,      DELIMITER,      0)  \
    X("}",              CLOSE_CBRACKET,     DELIMITER,      0)  \
    X("(",              OPEN_BRACKET,       DELIMITER,      0)  \
    X(")",              CLOSE_BRACKET,      DELIMITER,      0)  \
    X(DELIMITER_SYMBOL, DELIMITER,          DELIMITER,      0)  \
    X("if",             IF,                 RESERVED_WORD,  0)  \
    X("else",           ELSE,               RESERVED_WORD,  0)  \
    X("while",          WHILE,              RESERVED_WORD,  0)  \
    X("return",         RETURN,             RESERVED_WORD,  0)

#endif