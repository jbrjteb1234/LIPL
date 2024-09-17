#include <stdlib.h>
#include <stdio.h>
#include "tables/tables.h"
#include "ast_utility/ast.h"
#include "table_initiator.h"
#include "ast_utility/token_scanner.h"

#define T initiating_token
#define TT token_type
#define TV token_value

#define PT push_token_into_ast_node(iterator, initiating_token);
#define AT advance_token(initiating_token);

void initiate_statement(token** initiating_token, table_iterator* iterator){
    switch((*T)->TT){
        case RESERVED_WORD: {
            
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch ((*T)->TV.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                case VAR:

                    //DESIRED TOKENSTREAM: VAR ID =
                    //OUTPUT INTO NODESTACK: ID = 
                    //OUTCOME FSM: RES TABLE, STATE 0

                    AT
                    if( (*T)->TT !=  IDENTIFIER) {break;}
                    PT
                    AT
                    if( (*T)->TT != OPERATOR || (*T)->TV.operator_token_value != ASSIGNMENT ){break;} 


                    iterator->current->table = *get_reserved_table();
                    iterator->current->type = RESERVED_TABLE;
                    iterator->current->state = 0;

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
            iterator->current->table = *get_numbers_table();
            iterator->current->type = NUMBERS_TABLE;
            return;
        }
            
        case IDENTIFIER:{
            iterator->current->table = *get_assignment_table();
            iterator->current->type = ASSIGNMENT_TABLE;
            return;
        }

        default:
            break;
    }
    perror("Unexpected token before FSM parsing");
}