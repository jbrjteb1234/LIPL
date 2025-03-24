#ifndef TOKEN_TYPE
#define TOKEN_TYPE

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
} reserved_word_token;

typedef enum{
    EOS =0,
    OPEN_BRACKET = 1,
    CLOSE_BRACKET = 2,
    OPEN_CBRACKET = 3,
    CLOSE_CBRACKET = 4,
    COMMA = 5,
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
    RESERVED_WORD = 0,
    OPERATOR = 1,
    DELIMITER = 2,
    STRING_LITERAL = 3,
    INT_VALUE = 4,
    IDENTIFIER = 5
} token_types;

#endif