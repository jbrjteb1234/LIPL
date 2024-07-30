#include <stdio.h>
#include <stdlib.h>
#include "../utility/safe_memory.h"
#include "lexer_main.h"
#include "lexeme.h"
#include "token_type.h"
#include "token.h"

token* tokenize(FILE* source_code){
    //iterate through file, feeding each character into a buffer
    int c;
    bool eof_flag = false;

    character_buffer* buf = create_character_buffer();
    char* lexeme_buffer = NULL;

    token* current = NULL;
    tokentype_dictionary* dictionary = initialize_tokentype_dictionary();
    

    while (!eof_flag) {
        if((c = fgetc(source_code))==EOF){eof_flag = true;}
        if (produce_lexeme(buf, &lexeme_buffer, c) == true){

            token* temp_new_token = produce_token(current, dictionary, lexeme_buffer);
            if (temp_new_token != NULL){
                current = temp_new_token;
            }

            free(lexeme_buffer);
            lexeme_buffer = NULL;
        }
        insert_to_character_buffer(buf, c);
    }

    //placeholder until tokenization is complete.
    token* a;
    return a;
}