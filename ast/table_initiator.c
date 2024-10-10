#include <stdlib.h>
#include <stdio.h>
#include "tables/tables.h"
#include "ast_utility/ast.h"
#include "table_initiator.h"
#include "ast_utility/token_scanner.h"
#include "reducer.h"
#include "../utility/stack.h"

#define T_TYPE (*initiating_token)->token_type
#define T_VAL (*initiating_token)->token_value

#define PUSH push_token_into_ast_node(iterator, initiating_token);
#define ADV advance_token(initiating_token);

void initiate_statement(token** initiating_token, table_iterator* iterator){
    switch(T_TYPE){
        case RESERVED_WORD: {
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch (T_VAL.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                case VAR:

                    //DESIRED TOKENSTREAM: DECL =       (DECL = VAR ID)
                    //OUTPUT INTO NODESTACK: DECL = 
                    //OUTCOME FSM: RES TABLE, STATE 1
                    ADV
                    if( T_TYPE !=  IDENTIFIER) {break;}
                    PUSH
                    ASTNode* identifier = peek(iterator->node_stack);
                    identifier->type = LEAF_NODE;
                    identifier->value.leaf_node_value = DEC_NODE;
                    identifier->data.value_node.identifier = T_VAL.identifier_token_value;
                    ADV
                    if( T_TYPE != OPERATOR || T_VAL.operator_token_value != ASSIGNMENT ){break;} 

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
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            break;
        }
        case INT_VALUE:{
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            return;
        }
            
        case IDENTIFIER:{
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            return;
        }

        default:
            break;
    }
    perror("Unexpected token before FSM parsing");
}