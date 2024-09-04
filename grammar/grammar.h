#ifndef GRAMMAR
#define GRAMMAR

//different types of operator commands
typedef enum{
    ADDITION        =0,
    SUBTRACTION     =1,
    MULTIPLICATION  =2,
    DIVISION        =3,
    ASSIGNMENT      =4,
    EQUIVALENT      =5,
    NOT_EQUIVALENT  =6,
    LESS_THAN       =7,
    GREATER_THAN    =8,
    GREATER_OR_EQUAL=9,
    LESS_OR_EQUAL   =10,
} operator_token;

//different types of reserved word commands
typedef enum{
    IF,
    ELSE,
    WHILE,
    RETURN
} reserved_word_token;

typedef enum{
    EOS,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    OPEN_CBRACKET,
    CLOSE_CBRACKET,
} delimiter_token;

//The token value - could be an enum for operators or reserved words, or a variable value
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    delimiter_token         delimiter_token_value;
    int                     identifier_token_value;
    void*                   variable_value;               
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

#define DELIMITER_SYMBOL ";"

//  LEXEME              TOKEN VALUE         TOKEN TYPE      PRECEDENCE 
#define DEFINE_TOKENS \
    X("=",              ASSIGNMENT,         operator_token_value, OPERATOR,       10) \
    X("==",             EQUIVALENT,         operator_token_value, OPERATOR,       9)  \
    X("<",              LESS_THAN,          operator_token_value, OPERATOR,       8)  \
    X(">",              GREATER_THAN,       operator_token_value, OPERATOR,       8)  \
    X("=<",             LESS_OR_EQUAL,      operator_token_value, OPERATOR,       8)  \
    X(">=",             GREATER_OR_EQUAL,   operator_token_value, OPERATOR,       8)  \
    X("+",              ADDITION,           operator_token_value, OPERATOR,       6)  \
    X("-",              SUBTRACTION,        operator_token_value, OPERATOR,       6)  \
    X("*",              MULTIPLICATION,     operator_token_value, OPERATOR,       5)  \
    X("/",              DIVISION,           operator_token_value, OPERATOR,       5)  \
    X("{",              OPEN_CBRACKET,      delimiter_token_value, DELIMITER,      0)  \
    X("}",              CLOSE_CBRACKET,     delimiter_token_value, DELIMITER,      0)  \
    X("(",              OPEN_BRACKET,       delimiter_token_value, DELIMITER,      0)  \
    X(")",              CLOSE_BRACKET,      delimiter_token_value, DELIMITER,      0)  \
    X(DELIMITER_SYMBOL, EOS,                delimiter_token_value, DELIMITER,      0)  \
    X("if",             IF,                 reserved_word_token_value, RESERVED_WORD, 0)  \
    X("else",           ELSE,               reserved_word_token_value, RESERVED_WORD, 0)  \
    X("while",          WHILE,              reserved_word_token_value, RESERVED_WORD, 0)  \
    X("return",         RETURN,             reserved_word_token_value, RESERVED_WORD, 0)

#endif