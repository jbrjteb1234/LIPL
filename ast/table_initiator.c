#include <stdlib.h>
#include <stdio.h>
#include "tables/tables.h"
#include "ast_utility/ast.h"
#include "table_initiator.h"
#include "ast_utility/token_scanner.h"
#include "reducer.h"
#include "../utility/stack.h"

#define T initiating_token
#define TT token_type
#define TV token_value

#define PT push_token_into_ast_node(iterator, T);
#define AT advance_token(T);

void initiate_statement(token** T, table_iterator* iterator){
    switch((*T)->TT){
        case RESERVED_WORD: {
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch ((*T)->TV.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                case VAR:

                    //DESIRED TOKENSTREAM: DECL =
                    //OUTPUT INTO NODESTACK: DECL = 
                    //OUTCOME FSM: RES TABLE, STATE 1
                    AT
                    if( (*T)->TT !=  IDENTIFIER) {break;}
                    PT
                    ASTNode* identifier = peek(iterator->node_stack);
                    identifier->type = DECLARATION_NODE;
                    identifier->data.value_node.identifier = identifier->token->token_value.identifier_token_value;
                    AT
                    if( (*T)->TT != OPERATOR || (*T)->TV.operator_token_value != ASSIGNMENT ){break;} 

                    iterator->current->table = *get_reserved_table();
                    iterator->current->type = RESERVED_TABLE;
                    iterator->current->state = VAR_ENTRY;

                    return;

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
            iterator->current->table = *get_var_table();
            iterator->current->type = VAR_TABLE;
            break;
        }
        case INT_VALUE:{
            iterator->current->table = *get_var_table();
            iterator->current->type = VAR_TABLE;
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