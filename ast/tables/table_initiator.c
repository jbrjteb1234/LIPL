#include <stdlib.h>
#include <stdio.h>
#include "tables.h"
#include "../ast_utility/ast.h"
#include "table_initiator.h"
#include "../ast_utility/token_scanner.h"
#include "../reducer.h"
#include "../../utility/stack.h"
#include "../ast_utility/routines.h"
#include "../ast_utility/slist_functions.h"

#define T_TYPE (*initiating_token)->token_type
#define T_VAL (*initiating_token)->token_value

#define PUSH push_token_into_ast_node(iterator, initiating_token, false);
#define ADV advance_token(initiating_token);

void initiate_statement(token** initiating_token, table_iterator* iterator){
    switch(T_TYPE){
        case RESERVED_WORD: {
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch (T_VAL.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                case VAR: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    PUSH //PUSH IDENTIFIER INTO NODESTACK

                    ASTNode* identifier = peek(iterator->node_stack);
                    identifier->type = LEAF_NODE;
                    identifier->value.leaf_node_value = DEC_NODE;
                    identifier->data.value_node.identifier = T_VAL.identifier_token_value;

                    iterator->current->table = *get_reserved_table();
                    iterator->current->type = RESERVED_TABLE;

                    return;
                }
                case FUNC: {

                    //TODO: FUNCTION DECLERATION

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    PUSH //PUSH IDENTIFIER INTO NODESTACK

                    //convert identifier to function node
                    ASTNode* func_dec_node = *(ASTNode**)peek(iterator->node_stack); 
                    func_dec_node->type = RES_WORD_NODE;
                    func_dec_node->value.res_node_value = FUNCTION_DEC_NODE;
                    func_dec_node->data.function_node.identifier = T_VAL.identifier_token_value;


                    //LOAD RES TABLE

                    iterator->current->table = *get_reserved_table();
                    iterator->current->type = RESERVED_TABLE;
                    iterator->current->state = 3;

                    return;
                }
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

            switch (T_VAL.delimiter_token_value){
                case OPEN_BRACKET:

                    iterator->current->table = *get_expr_table();
                    iterator->current->type = EXPR_TABLE;
                    
                    iterator->current->state = open_expression_parentheses(iterator, O(14,0));

                    return;
            
                default:
                    break;
            }

            break;
        }
        case STRING_LITERAL:{
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            PUSH
            break;
        }
        case INT_VALUE:{
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            PUSH
            return;
        }
            
        case IDENTIFIER:{
            iterator->current->table = *get_expr_table();
            iterator->current->type = EXPR_TABLE;
            iterator->current->state = 1;
            PUSH
            return;
        }

        default:
            break;
    }
    perror("Unexpected token before FSM parsing");
}