#ifndef lexer_info
#define lexer_info

#define RO_COUNT 8

//different types of operator tokens
typedef enum{
    SUBTRACT,
    ADDITION,
    MULTIPLY,
    EQUIVALENT,
    ASSIGNMENT,
    OR
} operator_token;

//different types of reserved word tokens
typedef enum{
    INT_TYPE,
    IF,
    ELSE
} reserved_word_token;

//describes the type of token
typedef enum {
    RESERVED_WORD,
    OPERATOR,
    STRING_LITERAL,
    INT_VALUE,
    FLOAT_VALUE,
    IDENTIFIER
} token_functions;

//The token value - could be an enum for operators or reserved words, or a variable value
typedef union{
    operator_token          operator_token_value;
    reserved_word_token     reserved_word_token_value;
    int*                    number_token_value;
    float*                  float_token_value;
    char *                  string_literal_token_value;
    int                     identifier_token_value;                 
} token_values;

//token structure - one command
typedef struct{
    struct token* previous;
    struct token* next;
    
    token_functions token_type;
    token_values    token_value;
} token;

//populates the dictionary
typedef struct{
    token_values    token_value;
    char*           lexeme;
} tokentype_dictionary_entry;

typedef struct{
    int amount;
    int reserved_word_count;
    int reserved_dictionary_start;
    tokentype_dictionary_entry* dictionary;
} tokentype_dictionary;

void declare_identifier_in_tokentype_dictionary(tokentype_dictionary*, char*);

token_values token_check(char* lexeme);

#endif