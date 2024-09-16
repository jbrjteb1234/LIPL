#include <stdlib.h>
#include <stdio.h>
#include "tables/tables.h"
#include "ast_utility/ast.h"
#include "table_initiator.h"


table_type find_tabletype_from_token(token* initiating_token){
    switch(initiating_token->token_type){
        case RESERVED_WORD: {
            
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch (initiating_token->token_value.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS
                case VAR:
                    break;

                case FUNC:
                    break;

                case IF:
                    break;
                
                case ELSE:
                    break;

                case WHILE:
                    break;

                case RETURN:
                    break;
            }

            break;
        }
        case OPERATOR:{
            break;
        }
        case DELIMITER:{
            break;
        }
        case STRING_LITERAL:{
            break;
        }
        case INT_VALUE:{
            return NUMBERS_TABLE;
        }
            
        case IDENTIFIER:{
            //TODO: PROPERLY IMPLEMENT DECLERATION TABLES
            return DECL_TABLE;
        }

        default:
            break;
    }
    perror("Invalid token type");
    return NONE;
}