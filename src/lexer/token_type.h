#ifndef LIPL_LEXER_TOKEN_TYPE_H
#define LIPL_LEXER_TOKEN_TYPE_H

#include <stdint.h>

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
    DOT             =11,
    POWER           =12,
    ROOT            =13,
    AND             =14,
    OR              =15,
    NOT             =16,
    MODULO          =17,
} operator_token;

//different types of reserved word commands
typedef enum{
    VAR = 0,
    FUNC = 1,
    IF = 2,
    WHILE = 3,
    ELSE = 4,
    ELIF = 5,
    RETURN = 6,
    GLOBAL = 7,
    CONST = 8,
    FOR = 9,
    BOOL_TRUE = 10,
    BOOL_FALSE = 11,
    BREAK = 12,
    CONTINUE = 13,
} reserved_word_token;

typedef enum{
    EOS =0,
    OPEN_BRACKET = 1,
    CLOSE_BRACKET = 2,
    OPEN_CBRACKET = 3,
    CLOSE_CBRACKET = 4,
    COMMA = 5,
} delimiter_token;

//The token value - which member is valid depends on the token type
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    delimiter_token         delimiter_token_value;
    int                     identifier_token_value;   /* id from the symbol table */
    int64_t                 int_value;
    double                  float_value;
    char*                   string_value;             /* unescaped, owned by the token stream */
} token_values;

//describes the type of commands
typedef enum {
    RESERVED_WORD = 0,
    OPERATOR = 1,
    DELIMITER = 2,
    STRING_LITERAL = 3,
    INT_VALUE = 4,
    IDENTIFIER = 5,
    FLOAT_VALUE = 6,
    END_OF_INPUT = 7
} token_types;

#endif /* LIPL_LEXER_TOKEN_TYPE_H */
