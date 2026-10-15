#ifndef LIPL_GRAMMAR_H
#define LIPL_GRAMMAR_H

#include "../lexer/token_type.h"

/* Everything from this marker to the end of the line is ignored */
#define LINE_COMMENT "//"

/* Every fixed lexeme in the language. The lexer always takes the longest match,
   so "<=" wins over "<". Words ("if", "_") are matched as whole identifiers. */
//  LEXEME              TOKEN VALUE         TOKENS TYPE
#define DEFINE_TOKENS \
    X("=",              ASSIGNMENT,         operator_token_value,       OPERATOR)   \
    X("==",             EQUIVALENT,         operator_token_value,       OPERATOR)   \
    X("!=",             NOT_EQUIVALENT,     operator_token_value,       OPERATOR)   \
    X("<",              LESS_THAN,          operator_token_value,       OPERATOR)   \
    X(">",              GREATER_THAN,       operator_token_value,       OPERATOR)   \
    X("<=",             LESS_OR_EQUAL,      operator_token_value,       OPERATOR)   \
    X(">=",             GREATER_OR_EQUAL,   operator_token_value,       OPERATOR)   \
    X("+",              ADDITION,           operator_token_value,       OPERATOR)   \
    X("-",              SUBTRACTION,        operator_token_value,       OPERATOR)   \
    X("*",              MULTIPLICATION,     operator_token_value,       OPERATOR)   \
    X(".",              DOT,                operator_token_value,       OPERATOR)   \
    X("/",              DIVISION,           operator_token_value,       OPERATOR)   \
    X("%",              MODULO,             operator_token_value,       OPERATOR)   \
    X("^",              POWER,              operator_token_value,       OPERATOR)   \
    X("_",              ROOT,               operator_token_value,       OPERATOR)   \
    X("&&",             AND,                operator_token_value,       OPERATOR)   \
    X("||",             OR,                 operator_token_value,       OPERATOR)   \
    X("!",              NOT,                operator_token_value,       OPERATOR)   \
    X(",",              COMMA,              delimiter_token_value,      DELIMITER)  \
    X("{",              OPEN_CBRACKET,      delimiter_token_value,      DELIMITER)  \
    X("}",              CLOSE_CBRACKET,     delimiter_token_value,      DELIMITER)  \
    X("(",              OPEN_BRACKET,       delimiter_token_value,      DELIMITER)  \
    X(")",              CLOSE_BRACKET,      delimiter_token_value,      DELIMITER)  \
    X(";",              EOS,                delimiter_token_value,      DELIMITER)  \
    X("if",             IF,                 reserved_word_token_value,  RESERVED_WORD)  \
    X("else",           ELSE,               reserved_word_token_value,  RESERVED_WORD)  \
    X("elif",           ELIF,               reserved_word_token_value,  RESERVED_WORD)  \
    X("while",          WHILE,              reserved_word_token_value,  RESERVED_WORD)  \
    X("for",            FOR,                reserved_word_token_value,  RESERVED_WORD)  \
    X("return",         RETURN,             reserved_word_token_value,  RESERVED_WORD)  \
    X("break",          BREAK,              reserved_word_token_value,  RESERVED_WORD)  \
    X("continue",       CONTINUE,           reserved_word_token_value,  RESERVED_WORD)  \
    X("func",           FUNC,               reserved_word_token_value,  RESERVED_WORD)  \
    X("var",            VAR,                reserved_word_token_value,  RESERVED_WORD)  \
    X("global",         GLOBAL,             reserved_word_token_value,  RESERVED_WORD)  \
    X("const",          CONST,              reserved_word_token_value,  RESERVED_WORD)  \
    X("true",           BOOL_TRUE,          reserved_word_token_value,  RESERVED_WORD)  \
    X("false",          BOOL_FALSE,         reserved_word_token_value,  RESERVED_WORD)

#endif /* LIPL_GRAMMAR_H */
