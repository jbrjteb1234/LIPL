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
#define PUSH_AUTO push_token_into_ast_node(iterator, initiating_token, true);
#define ADV advance_token(initiating_token);

void initiate_statement(token** initiating_token, table_iterator* iterator){
    switch(T_TYPE){
        case RESERVED_WORD: {
            //RESERVED WORD - CHECK WHICH RESERVED WORD IT IS
            switch (T_VAL.reserved_word_token_value){
                //TODO: IMPLEMENT RESERVED WORDS

                //VARIABLE DECLERATION
                ASTNode* current_node;
                case FUNC: {

                    ADV //ADV TO IDENTIFIER

                    if( T_TYPE !=  IDENTIFIER) {break;}

                    //convert identifier to function node
                    current_node = PUSH //PUSH IDENTIFIER IN TO NODE STACK
                    current_node->type = FUNC_NODE;
                    current_node->value.func_node_value = FUNC_DEC_NODE;
                    current_node->data.function_node.identifier = T_VAL.identifier_token_value;
                    current_node->block_flag=true;


                    //set state in res table
                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;
                    iterator->state = 0; 

                    return;
                }
                case ELIF:
                case IF:
                case WHILE:

                    current_node = PUSH
                    current_node->type = CONDITIONAL_BLOCK_NODE;
                    current_node->data.conditional_block_node.alternate = NULL;
                    
                    switch(T_VAL.reserved_word_token_value){
                        case IF:
                            current_node->value.conditional_block_node_value = IF_NODE;
                            break;
                        case WHILE:                        
                            current_node->value.conditional_block_node_value = WHILE_NODE;
                            break;
                        case ELIF:
                            attach_to_previous_conditional_block(iterator, current_node);
                            current_node->value.conditional_block_node_value = ELIF_NODE;
                            break;
                        default:
                            break;
                    }
                    
                    current_node->block_flag=true;

                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;
                    iterator->state = 2;                    

                    return;

                case ELSE:

                    current_node = PUSH;

                    attach_to_previous_conditional_block(iterator, current_node);

                    current_node->type = CONDITIONAL_BLOCK_NODE;
                    current_node->value.conditional_block_node_value = ELSE_NODE;
                    current_node->block_flag=true;

                    iterator->table = iterator->reserved_table;
                    iterator->type = RESERVED_TABLE;
                    iterator->state = 4;   

                    return;
                
                case RETURN:

                    current_node = PUSH;

                    //todo implement

                    break;
                
                case VAR:
                case GLOBAL:
                case CONST:

                    set_specifiers(iterator, T_VAL.reserved_word_token_value);
                    ADV;

                    initiate_statement(initiating_token, iterator);

                    return;

            }

            break;
        }
        case OPERATOR:{
            break;
        }
        case DELIMITER:{

            switch (T_VAL.delimiter_token_value){
                case OPEN_BRACKET:

                    iterator->table = iterator->expr_table;
                    iterator->type = EXPR_TABLE;
                    
                    iterator->state = open_expression_parentheses(iterator, O(EXPR_OPENPAREN_STATE,0));

                    return;

                default:
                    break;
            }

            break;
        }
        case STRING_LITERAL:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            PUSH_AUTO
            break;
        }
        case INT_VALUE:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            PUSH_AUTO
            return;
        }
            
        case IDENTIFIER:{
            iterator->table = iterator->expr_table;
            iterator->type = EXPR_TABLE;
            iterator->state = 1;
            PUSH_AUTO
            return;
        }

        default:
            break;
    }
    perror("Unexpected token before FSM parsing");
}

/** initiates iterator with a new type of table
 * 
 */
void initiate_table(table_iterator* iterator, token** initiating_token){
    printf("Initiating\n");

    iterator->state = 0;
    iterator->table = NULL;
    iterator->type = NONE_TABLE;

    initiate_statement(initiating_token, iterator);

    iterator->initiated = 1;

    advance_token(initiating_token);
}