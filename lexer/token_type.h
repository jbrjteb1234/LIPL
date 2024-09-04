#ifndef TOKEN_TYPE
#define TOKEN_TYPE

#define INITIAL_DICTIONARY_SIZE 50
#define DICTIONARY_EXPAND_AMOUNT 10

#include "token.h"
#include "lexeme.h"

//populates the dictionary - links string lexeme value token_value and token_function
typedef struct{
    token_values    token_value;
    token_types     token_type;

    unsigned char precedence;

    char*           lexeme;
} tokentype_dictionary_entry;

//dictionary of tokentypes
typedef struct{
    int maximum_amount;
    char* free_list;

    int identifier_count;

    tokentype_dictionary_entry** dictionary;
} tokentype_dictionary;

token* produce_token(token* prev, tokentype_dictionary* dictionary, lexeme* lexeme);

tokentype_dictionary* initialize_tokentype_dictionary();

void create_new_tokentype(tokentype_dictionary*, char*, token_values, token_types, unsigned char);

#endif

