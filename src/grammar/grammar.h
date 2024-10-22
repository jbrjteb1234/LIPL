#ifndef GRAMMAR
#define GRAMMAR

#include "../lexer/token_type.h"

#define DELIMITER_SYMBOL ";"

//  LEXEME              TOKEN VALUE         TOKEN TYPE 
#define DEFINE_TOKENS \
    X("=",              ASSIGNMENT,         operator_token_value,       OPERATOR)   \
    X("==",             EQUIVALENT,         operator_token_value,       OPERATOR)   \
    X("<",              LESS_THAN,          operator_token_value,       OPERATOR)   \
    X(">",              GREATER_THAN,       operator_token_value,       OPERATOR)   \
    X(">=",             LESS_OR_EQUAL,      operator_token_value,       OPERATOR)   \
    X(">=",             GREATER_OR_EQUAL,   operator_token_value,       OPERATOR)   \
    X("+",              ADDITION,           operator_token_value,       OPERATOR)   \
    X("-",              SUBTRACTION,        operator_token_value,       OPERATOR)   \
    X("*",              MULTIPLICATION,     operator_token_value,       OPERATOR)   \
    X(".",              DOT,                operator_token_value,       OPERATOR)   \
    X("/",              DIVISION,           operator_token_value,       OPERATOR)   \
    X(",",              COMMA,              delimiter_token_value,      DELIMITER)  \
    X("{",              OPEN_CBRACKET,      delimiter_token_value,      DELIMITER)  \
    X("}",              CLOSE_CBRACKET,     delimiter_token_value,      DELIMITER)  \
    X("(",              OPEN_BRACKET,       delimiter_token_value,      DELIMITER)  \
    X(")",              CLOSE_BRACKET,      delimiter_token_value,      DELIMITER)  \
    X(";",              EOS,                delimiter_token_value,      DELIMITER)  \
    X("if",             IF,                 reserved_word_token_value,  RESERVED_WORD)   \
    X("else",           ELSE,               reserved_word_token_value,  RESERVED_WORD)   \
    X("elif",           ELIF,               reserved_word_token_value,  RESERVED_WORD)   \
    X("while",          WHILE,              reserved_word_token_value,  RESERVED_WORD)   \
    X("return",         RETURN,             reserved_word_token_value,  RESERVED_WORD)   \
    X("func",           FUNC,               reserved_word_token_value,  RESERVED_WORD)   \
    X("var",            VAR,                reserved_word_token_value,  RESERVED_WORD)   

#endif