#ifndef TOKEN_DICT
#define TOKEN_DICT

#define INITIAL_DICTIONARY_SIZE 50
#define DICTIONARY_EXPAND_AMOUNT 10

#include "token.h"
#include "lexeme.h"
#include "../utility/data_pool.h"

#include <stdint.h>

/* Dictionary entry: maps a lexeme string to a token type and a token_values union. */
typedef struct{
    token_values    token_value;
    token_types     token_type;
    char*           lexeme;    /* owned by the dictionary */
} tokentype_dictionary_entry;

typedef struct{
    uint32_t maximum_amount;
    char* free_list;                    /* 0 = free slot, 1 = occupied */
    int identifier_count;               /* next identifier ID to assign */
    tokentype_dictionary_entry** dictionary;
} tokentype_dictionary;

/* Lifecycle */
tokentype_dictionary* initialize_tokentype_dictionary(void);
void destroy_tokentype_dictionary(tokentype_dictionary* dictionary);

/* Lookup / mutation */
tokentype_dictionary_entry* tokentype_lookup(tokentype_dictionary* dictionary, const char* lexeme);
void expand_dictionary(tokentype_dictionary* dictionary);
void create_new_tokentype(tokentype_dictionary*, const char*, token_values, token_types);

/* Token construction
   NOTE:
   - For STRING_LITERAL / INT_VALUE, this function DEEP-COPIES the lexeme into the token.
   - For IDENTIFIER, this function assigns a stable integer ID from the dictionary
     and does NOT store a string pointer in the token. */
token* produce_token(token* prev, tokentype_dictionary* dictionary, lexeme* lex, data_pool* token_pool);

#endif /* TOKEN_DICT */

