#ifndef lexer_info
#define lexer_info
#include "../grammar/grammar.h"

//token structure - one command
typedef struct token{
    struct token* previous;
    struct token* next;
    
    unsigned char precedence;

    //Used later by the AST to link the token to the ASTNode, and thus link AST nodes to each other
    void*           ASTNode;
    
    token_types     token_type;
    token_values    token_value;
} token;

#endif