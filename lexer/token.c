#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"

/** functionality for managing the relation between lexemes and different types of tokens
 *  allows for declaring identifier tokens
 */

char* initialize_tokentype_dictionary(){
    // Initialize the pairs
    tokentype_dictionary dictionary;
    dictionary.amount=20;
    dictionary.reserved_word_count=0;
    dictionary.reserved_dictionary_start = RO_COUNT;

    int size = sizeof(tokentype_dictionary_entry)*dictionary.amount;
    dictionary.dictionary = (tokentype_dictionary_entry*)malloc(size);
    memset(&dictionary, 0, size);

    dictionary.dictionary[0] = (tokentype_dictionary_entry){SUBTRACT, strdup("-")};
    dictionary.dictionary[1] = (tokentype_dictionary_entry){ADDITION, strdup("+")};
    dictionary.dictionary[2] = (tokentype_dictionary_entry){MULTIPLY, strdup("*")};
    dictionary.dictionary[3] = (tokentype_dictionary_entry){EQUIVALENT, strdup("==")};
    dictionary.dictionary[4] = (tokentype_dictionary_entry){ASSIGNMENT, strdup("=")};
    dictionary.dictionary[5] = (tokentype_dictionary_entry){INT_TYPE, strdup("int")};
    dictionary.dictionary[6] = (tokentype_dictionary_entry){IF, strdup("if")};
    dictionary.dictionary[7] = (tokentype_dictionary_entry){ELSE, strdup("else")};
}

void declare_identifier_in_tokentype_dictionary(tokentype_dictionary* current_dictionary, char* identifier){
    int index = current_dictionary->reserved_dictionary_start;

    
}

token_values token_check(char* lexeme){
    
}